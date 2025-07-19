// ----- arduino_rgb_bot.ino -----
#include <Arduino.h>

const int pinR = 8;   // PWM-канал для красного
const int pinG = 9;   // PWM-канал для зелёного
const int pinB = 10;  // PWM-канал для синего

void setRGB(int r, int g, int b) {
  analogWrite(pinR, r);
  analogWrite(pinG, g);
  analogWrite(pinB, b);
}

void setup() {
  Serial.begin(9600);
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  // Изначально — свет выключен
  setRGB(0, 0, 0);
}

void loop() {
  if (Serial.available()) {
    // Индикатор: зелёный, когда приняли начало команды
    setRGB(0, 200, 0);
    delay(500);

    // Читаем строку до \n
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    // Разбираем формат "R,G,B"
    int values[3] = {0, 0, 0};
    int idx = 0;
    char *pch;
    char buf[20];
    cmd.toCharArray(buf, sizeof(buf));     // переводим в C-строку
    pch = strtok(buf, ",");
    while (pch != NULL && idx < 3) {
      values[idx++] = constrain(atoi(pch), 0, 255);
      pch = strtok(NULL, ",");
    }

    // Индикатор: синий, когда установили ШИМ
    setRGB(0, 0, 200);
    delay(500);

    // Устанавливаем требуемый цвет
    setRGB(values[0], values[1], values[2]);

    // Подтверждение
    Serial.print("OK:");
    Serial.print(values[0]); Serial.print(",");
    Serial.print(values[1]); Serial.print(",");
    Serial.println(values[2]);
  }

  // По умолчанию красный мигающий индикатор каждые 1 с
  setRGB(200, 0, 0);
  delay(1000);
  setRGB(0, 0, 0);
  delay(1000);
}
