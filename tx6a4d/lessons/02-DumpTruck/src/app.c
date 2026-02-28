#include "app.h"

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  int8_t v = getStick(index);
  if (v > deadzone) {
    return (float)(v - deadzone) / (127 - deadzone) * 127;
  } else if (v < -deadzone) {
    return (float)(v + deadzone) / (127 - deadzone) * 127;
  }
  return 0;
}

void loop() {
  setChannel(1, - getStickNeutral(2, 5) / 2); // Engine
  setChannel(2, - getStickNeutral(1, 0x40)); // Lift

  setChannel(3, - getStick(4)); // Led

  setChannel(4, getStickNeutral(0, 5) / 2); // Turn
  setChannel(5, getStick(5)); // differential Lock
}
