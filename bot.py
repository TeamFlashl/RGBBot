import asyncio
import threading
import time
import logging             # <-- стандартный модуль
import serial
import config

from aiogram import Bot, Dispatcher, types
from aiogram.filters import Command
from aiogram.utils.keyboard import ReplyKeyboardBuilder
from aiogram import F
from aiogram import Router

API_TOKEN = config.API_TOKEN
SERIAL_PORT = config.SERIAL_PORT
BAUDRATE = config.BAUDRATE

# ===== Инициализация порта =====
try:
    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
    print(f"Открыт порт {SERIAL_PORT} @ {BAUDRATE}")
    time.sleep(2)
except Exception as e:
    print("Не удалось открыть порт:", e)
    exit(1)

def start_serial():
    if not ser.is_open:
        try:
            ser.open()
            time.sleep(2)
            print(f"Открыт порт {SERIAL_PORT} @ {BAUDRATE}")
        except Exception as e:
            print("Не удалось открыть порт:", e)
            exit(1)

def stop_serial():
    if ser.is_open:
        ser.close()
        print(f"Закрыт порт {SERIAL_PORT}")


# ===== Структура меню =====
menuItems = {
    "buttonRed":        {"label": "🔴 Красный", "id": "red", "command": "mode 1", "visible": True},
    "buttonGreen":      {"label": "🟢 Зелёный", "id": "green", "command": "mode 2", "visible": True},
    "buttonBlue":       {"label": "🔵 Синий", "id": "blue", "command": "mode 3", "visible": True},
    "buttonWhite":      {"label": "⚪️ Белый", "id": "white", "command": "mode 7", "visible": True},
    "buttonBlack":      {"label": "⚫️ Чёрный", "id": "black", "command": "mode 0", "visible": True},
    "buttonYellow":     {"label": "🟡 Жёлтый", "id": "yellow", "command": "mode 4", "visible": True},
    "buttonMagenta":    {"label": "🟣 Розовый", "id": "magenta", "command": "mode 5", "visible": True},
    "buttonCyan":       {"label": "🩵 Голубой", "id": "cyan", "command": "mode 6", "visible": True},
}



# ===== Построение клавиатуры =====
def build_main_keyboard() -> types.ReplyKeyboardMarkup:
    kb_builder = ReplyKeyboardBuilder()
    for item in menuItems.values():
        if item["visible"]:
            # используем текст из поля "label"
            kb_builder.button(text=item["label"])
    kb_builder.adjust(5)  # по 5 кнопок в ряд
    return kb_builder.as_markup(resize_keyboard=True)

# ===== Работа с Arduino =====
def read_arduino_response(chat_id: int, bot: Bot):
    time.sleep(2)
    try:
        resp = ser.read_all().decode().strip()
        print(f"[Arduino] {resp}")
        # Чтобы отправить ответ юзеру, можно разкомментировать:
        # asyncio.run(bot.send_message(chat_id, f"Arduino: {resp}"))
    except Exception as e:
        print("Ошибка чтения:", e)

def send_to_arduino(cmd_value: int, chat_id: int, bot: Bot):
    try:
        ser.write(str(cmd_value).encode())
        thread = threading.Thread(
            target=read_arduino_response,
            args=(chat_id, bot),
            daemon=True
        )
        thread.start()
    except Exception as e:
        asyncio.create_task(bot.send_message(chat_id, f"Ошибка отправки: {e}"))

# ===== Маршруты =====
router = Router()

@router.message(Command("start"))
@router.message(Command("help"))
async def cmd_start(message: types.Message):
    kb = build_main_keyboard()
    await message.answer(
        "Привет! Управляю RGB-лентой.\n"
        "Выберите цвет кнопкой либо /rgb R G B",
        reply_markup=kb
    )

@router.message(Command("rgb"))
async def cmd_rgb(message: types.Message):
    parts = message.text.split()
    if len(parts) != 4:
        return await message.reply("Неверный формат, используйте: /rgb R G B")
    try:
        r, g, b = map(int, parts[1:])
        if any(not (0 <= v <= 255) for v in (r, g, b)):
            raise ValueError
    except ValueError:
        return await message.reply("Значения 0–255")
    cmd_str = f"rgb {r} {g} {b}"
    try:
        ser.write(cmd_str.encode())
        await message.reply(f"Отправил: {r},{g},{b}")
    except Exception as e:
        await message.reply(f"Ошибка: {e}")

@router.message()
async def choose_color(message: types.Message, bot: Bot):
    # ищем в menuItems по text == item["label"]
    selected = None
    for item in menuItems.values():
        if item["label"] == message.text:
            selected = item
            break

    if not selected:
        return await message.reply("Не понял, выберите цвет на клавиатуре")

    cmd_value = selected.get("command")
    if cmd_value is None:
        return await message.reply("Команда не задана")

    await message.reply(f"Выбрали {selected['label']}, шлём {cmd_value}")
    send_to_arduino(cmd_value, message.chat.id, bot)

# ===== Запуск приложения =====
async def main():
    # включаем логирование
    logging.basicConfig(level=logging.INFO)
    # создаём Bot и Dispatcher
    bot = Bot(token=API_TOKEN)
    dp = Dispatcher()
    # подключаем router
    dp.include_router(router)
    # запускаем polling
    print("Бот запущен")
    await dp.start_polling(bot)

if __name__ == "__main__":
    asyncio.run(main())