#include "app.h"

#define GEARS 5
uint8_t gear = GEARS;

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


  setChannel(4, getStickNeutral(0, 5) * 3 / 4 + getStick(5) / 4); // SM0

  setChannel(0, -getStickNeutral(2, 10) * gear / GEARS); // DM0

  setChannel(1, getStickNeutral(3, 20)); // DM1

  setChannel(2, getStickNeutral(1, 20)); // DM2

  setChannel(5, getStick(4)); // SM1

  if (getButton(0) && !buttonsLast[0]) {
    if (gear > 1) gear--;
  }

  if (getButton(1) && !buttonsLast[1]) {
    if (gear < GEARS) gear++;
  }

  if (getButton(2) && !buttonsLast[2]) {
    if (getChannel(3)) {
      setChannel(3, 0);
    } else {
      setChannel(3, -100);
    }
  }

  if (getButton(3) && !buttonsLast[3]) {
    if (getChannel(3)) {
      setChannel(3, 0);
    } else {
      setChannel(3, 100);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
