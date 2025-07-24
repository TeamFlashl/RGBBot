#include "CommandHandler.h"
#include "Analog.h"

void handleSerialCommands(String command, volatile int *ledMode, RGB8 *currentColor) {  
  setRGB(255,255,255);  
  command.trim();           
  if (command.length() == 0) return;

  // Split the line into tokens
  // first token = command, subsequent tokens = args
  int idx = command.indexOf(' ');
  String cmd = (idx < 0) ? command : command.substring(0, idx);
  cmd.toLowerCase();

  String args = (idx < 0) ? String() : command.substring(idx + 1);
  args.trim();

  // DEBUG INFO
  Serial.print("Command: ");
  Serial.print(cmd);
  Serial.print(" Args: ");
  Serial.println(args);

  if (cmd == "restart") {
    Serial.println("Command: restart");
    //restartController();
  }
  else if (cmd == "mode") {
    // expect a single integer parameter
    int m = args.toInt();
    Serial.print("Command: mode ");
    Serial.println(m);
    *ledMode = m;
  }
  else if (cmd == "rgb") {
    // expect three integers: R G B
    uint8_t r, g, b;
    // scan args
    int cnt = sscanf(args.c_str(), "%hhu %hhu %hhu", &r, &g, &b);
    if (cnt == 3) {
      *ledMode = 999;
      *currentColor = { r, g, b };
    } else {
      Serial.println("Error: rgb requires 3 parameters");
    }
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
  }
}
