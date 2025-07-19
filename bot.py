# ----- bot.py -----
import cmd
import config
import telebot
import serial
import time 

from config import TOKEN

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
    # exit(1)

# Простая клавиатура
from telebot.types import ReplyKeyboardMarkup, KeyboardButton

markup = ReplyKeyboardMarkup(resize_keyboard=True)
markup.row(KeyboardButton("🔴 Красный"), KeyboardButton("🟢 Зелёный"), KeyboardButton("🔵 Синий"))
markup.row(KeyboardButton("⚪️ Белый"), KeyboardButton("⚫️ Чёрный"))
markup.row(KeyboardButton("🔴 мигалка 🔵"))


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
        return
    # send_to_arduino(r, g, b, message)
    M_send_to_arduino(type, message)

@bot.message_handler(func=lambda m: True)
def choose_color(message):
    text = message.text.lower()
    bot.reply_to(message, "Вы выбрали цвет: " + text)

    if "красн" in text:
        M_send_to_arduino(1, message)
    elif "зелёный мигалка" in text:
        M_send_to_arduino(130, message)
    elif "зелён" in text:
        M_send_to_arduino(2, message)
    elif "син" in text:
        M_send_to_arduino(3, message)
    elif "бел" in text:
        M_send_to_arduino(4, message)
    elif "выкл" in text:
        M_send_to_arduino(0, message)
    elif "мигалка" in text:
        M_send_to_arduino(130, message)
    elif "131" in text:
        M_send_to_arduino(131, message)
    else:
        M_send_to_arduino(text, message)
        # bot.reply_to(message, "Не понял команду, выберите цвет на клавиатуре или /rgb R G B")

"""
def send_mode1_to_arduino(message): # TODO ??
    try:
        ser.write(b"mode1\n")
        time.sleep(0.1)
        resp = ser.readline().decode().strip()
        if resp.startswith("OK"):
            bot.reply_to(message, "Включён режим: Зелёный мигалка")
        else:
            bot.reply_to(message, "Нет ответа от Arduino")
    except Exception as e:
        bot.reply_to(message, f"Ошибка отправки на Arduino: {e}")
"""

def M_send_to_arduino(type, message):
    cmd = f"{type}\n"  # Формируем команду для Arduino
    try:
        print(f"Отправка на Arduino: {cmd.strip()}")
        ser.write(cmd.encode())  # TODO TEST отправить число 3
        print(f"Отправлено на Arduino: {cmd.strip()}") # 10/5
        # Ждем ответа от Arduino
        time.sleep(0.1)
        resp = ser.readline().decode().strip()
        if resp.startswith("OK"):
            bot.reply_to(message, f"Установлен цвет TYPE={type}")
        else:
            bot.reply_to(message, "Нет ответа от Arduino")
    except Exception as e:
        bot.reply_to(message, f"Ошибка отправки на Arduino: {e}")


def sum(a, b):  
    return a + b    

if __name__ == '__main__':
    print("Бот запущен...")
    ser.write(b"131\n")  # Отправляем команду на Arduino для установки цвета CYAN
    time.sleep(0.1)  # Ждем ответа от Arduino
    bot.infinity_polling()
