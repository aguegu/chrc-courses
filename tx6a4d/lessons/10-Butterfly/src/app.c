#include "app.h"

#define DEADZONE (3)

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
  static bool buttonsLast[4] = { false, false, false, false };

  setChannel(0, getStickNeutral(0));
  setChannel(1, getStickNeutral(1));

  setChannel(2, getStick(2));

  setChannel(3, getStickNeutral(3));

  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(0) && !buttonsLast[0]) {
    if (getChannel(6) < 127) {
      setChannel(6, getChannel(6) + 1);
    }
  }

  if (getButton(1) && !buttonsLast[1]) {
    if (getChannel(6) > -127) {
      setChannel(6, getChannel(6) - 1);
    }
  }

  if (getButton(2) && !buttonsLast[2]) {
    if (getChannel(7) < 127) {
      setChannel(7, getChannel(7) + 1);
    }
  }

  if (getButton(3) && !buttonsLast[3]) {
    if (getChannel(7) > -127) {
      setChannel(7, getChannel(7) - 1);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
