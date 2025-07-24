#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>

// you must implement or extern these in your main .ino:
extern volatile int ledMode;
extern void restartController(); // TODO

struct RGB8 { uint8_t r, g, b; };
extern RGB8 currentColor;

// Call this from your loop()
void handleSerialCommands(String command, volatile int *ledMode, RGB8 *currentColor);

#endif // COMMAND_HANDLER_H
