// The main logic control is at the receiver/rx/vehicle side

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
  setChannel(0, getStickNeutral(0, 4));
  setChannel(1, getStickNeutral(1, 4));
  setChannel(2, getStickNeutral(2, 4));
  setChannel(3, getStickNeutral(3, 4));

  setChannel(8, getStick(4));
  setChannel(9, getStick(5));

  setChannel(10, getButton(0));
  setChannel(11, getButton(1));
  setChannel(12, getButton(2));
  setChannel(13, getButton(3));
}
