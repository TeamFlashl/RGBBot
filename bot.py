# ----- bot.py -----
import cmd
import config
import telebot
import serial
import time 

from config import TOKEN
# Простая клавиатура
from telebot.types import ReplyKeyboardMarkup, KeyboardButton
import threading

# Настройки
TOKEN = config.TOKEN
SERIAL_PORT = "COM3"   # или COM3 на Windows
BAUDRATE = 9600

# Инициализация
bot = telebot.TeleBot(TOKEN)
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    print(f"Открыт последовательный порт: {SERIAL_PORT} с baudrate {BAUDRATE}")
    time.sleep(2)  # дать Arduino время перезагрузиться
except Exception as e:
    print("Не получается открыть последовательный порт:", e)
    exit(1)


# Define menu items and their commands
menuItems = {
    "🔴 Красный": {"command": 1,"visible": True},
    "🟢 Зелёный": {"command": 2,"visible": True},
    "🔵 Синий": {"command": 3,"visible": True},
    "⚪️ Белый": {"command": 7,"visible": True},
    "⚫️ Чёрный": {"command": 0,"visible": True},
    "📁 Папка": {"visible": False, "submenu": {
        "⚪️ Белый": {"command": 7,"visible": True},
        "⚫️ Чёрный": {"command": 0,"visible": True}
    }}
}

markup = ReplyKeyboardMarkup(resize_keyboard=True)
markup.max_row_keys = 3  # Количество кнопок в строке
for text, item in menuItems.items():
    if item["visible"]:
        markup.add(KeyboardButton(text))

@bot.message_handler(commands=['start', 'help'])
def send_welcome(message):
    bot.send_message(message.chat.id,
                     "Привет! Я управлю RGB-лентой.\n"
                     "Выберите готовый цвет или задайте свой через /rgb R G B",
                     reply_markup=markup)

@bot.message_handler(commands=['rgb'])
def custom_rgb(message):
    parts = message.text.split()
    if len(parts) != 4:
        bot.reply_to(message, "Неверный формат. Используйте: /start")
        return
    try:
        r, g, b = map(int, parts[1:])
        for v in (r, g, b):
            if not 0 <= v <= 255:
                raise ValueError
    except:
        bot.reply_to(message, "Значения должны быть числами 0–255")
    if text in menuItems:
        M_send_to_arduino(menuItems[text]["command"], message)

@bot.message_handler(func=lambda m: True)
def choose_color(message):
    text = message.text
    bot.reply_to(message, "Вы выбрали цвет: " + text)

    if menuItems.get(text):
        M_send_to_arduino(menuItems.get(text)["command"], message)
    else:
        bot.reply_to(message, "Не понял команду, выберите цвет на клавиатуре")

def read_arduino_response(message):
    try:
        time.sleep(2)
        #resp = ser.readline().decode().strip()
        resp = ser.read_all().decode().strip()
        print(f"Ответ от Arduino для отладки: {resp}")
        # Uncomment if you want to send response back to user
        #if resp["Response"]:
        #    bot.reply_to(message, f"Ответ от Arduino: {resp["Response"]}")
    except Exception as e:
        print(f"Ошибка чтения с Arduino: {e}")

def M_send_to_arduino(type, message):
    cmd = f"{type}"  # Формируем команду для Arduino
    try:
        ser.write(cmd.encode())
        # Start response reading in separate thread
        thread = threading.Thread(target=read_arduino_response, args=(message,))
        thread.daemon = True
        thread.start()
    except Exception as e:
        bot.reply_to(message, f"Ошибка отправки на Arduino: {e}")

if __name__ == '__main__':
    print("Бот запущен...")

    time.sleep(0.1)  # Ждем ответа от Arduino
    bot.infinity_polling()