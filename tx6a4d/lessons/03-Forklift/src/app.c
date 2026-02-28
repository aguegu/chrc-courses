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

int8_t stickToMotor(uint8_t index, uint8_t deadzone) {
  double f = getStickNeutral(index, deadzone) / 127.0;
  return (int8_t)(f * f * 127.0 * (f > 0 ? 1 : -1));
}

void loop() {
  setChannel(0, stickToMotor(3, 5) / 2); // Pitch
  setChannel(1, - stickToMotor(2, 5) / 2); // Engine
  setChannel(2, stickToMotor(1, 5)); // Lift

  setChannel(3, - getStick(4)); // Led

  setChannel(4, getStick(0) / 2); // Turn

  setChannel(6, getButton(3));  // Start
}
