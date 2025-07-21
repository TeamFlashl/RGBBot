#pragma once
//#include <Arduino.h>
#include "analog.h"

const int pinR = 8;   // PWM-канал для красного
const int pinG = 9;   // PWM-канал для зелёного
const int pinB = 10;  // PWM-канал для синего
extern int r, g, b;

void setupAnalog();
void setRGB(int _r, int _g, int _b);
/*
void loop() {
    setRGB(0, 0, 255);
    delay(1000);
    setRGB(0, 255, 0);
    delay(1000);
}
*/