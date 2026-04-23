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

const uint8_t freq = 8; // 1, 2, 4, 8, 16, 32, 64
bool isLedOn = false;
bool isBlinking = false;
const int8_t ledBrightness = 127;

void loop() {
  static uint8_t step = 0;
  static bool buttonsLast[4];

  setChannel(0, getStickNeutral(0, 6));
  setChannel(1, getStickNeutral(1, 6));
  setChannel(2, getStickNeutral(2, 4));

  setChannel(8, getStick(4));
  setChannel(9, getStick(5));

  setChannel(10, getButton(0));
  setChannel(11, getButton(1));

  if (getButton(2) && !buttonsLast[2]) {
    isBlinking = !isBlinking;
  }

  if (getButton(3) && !buttonsLast[3]) {
    isLedOn = !isLedOn;
  }

  if (isLedOn) {
    setChannel(3, ledBrightness);
    if (isBlinking) {
      setChannel(3, (step++ & freq) ? ledBrightness : 0);
    }
  } else {
    setChannel(3, 0);
  }

  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
