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
  setChannel(0, getStickNeutral(2, 5));
  setChannel(1, getStickNeutral(3, 5));
  setChannel(2, getStickNeutral(0, 5));
  setChannel(3, getStickNeutral(1, 5));

  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(0) == getButton(1)) {
    setChannel(6, 0);
  } else if (getButton(1)) {
    setChannel(6, 127);
  } else {  // getButton(0)
    setChannel(6, -127);
  }

  if (getButton(2) == getButton(3)) {
    setChannel(7, 0);
  } else if (getButton(2)) {
    setChannel(7, 127);
  } else {  // getButton(3)
    setChannel(7, -127);
  }
}
