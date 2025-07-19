#include "analog.h"

int r, g, b;

void setupAnalog() {
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  setRGB(0, 0, 0);
}

void setRGB(int _r, int _g, int _b) {
  analogWrite(pinR, _r);
  analogWrite(pinG, _g);
  analogWrite(pinB, _b);
  r = _r;
  g = _g;
  b = _b;
  
   // Сохраняем значения RGB для отладки
  //Отладочная строка для вывода значений RGB
  //Serial.print("RGB: ");
  //String out = String(r) + ", " + String(g) + ", " + String(b);
  //Serial.println(String(r) + ", " + String(g) + ", " + String(b));
  //delay(500);
}