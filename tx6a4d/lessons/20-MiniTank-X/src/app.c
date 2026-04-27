#include "app.h"

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#define STK_Y 1
#define STK_X 0
#define STK_SPEED 4

#define CHN_L 0
#define CHN_R 1

int16_t maxIn3(int16_t a, int16_t b, int16_t c) {
  int16_t m = MAX(a, b);
  return MAX(m, c);
}

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  int8_t v = getStick(index);
  if (v > deadzone) {
    return (v - deadzone) * 127 / (127 - deadzone);
  } else if (v < -deadzone) {
    return (v + deadzone) * 127 / (127 - deadzone);
  }
  return 0;
}

void loop() {
  int16_t left = getStickNeutral(STK_Y, 5) - getStickNeutral(STK_X, 5);
  int16_t right = getStickNeutral(STK_Y, 5) + getStickNeutral(STK_X, 5);

  uint16_t m = maxIn3(abs(left), abs(right), 127);

  uint16_t speed = (getStick(STK_SPEED) + 127) / 2;

  setChannel(CHN_R, right * speed / m);
  setChannel(CHN_L, left * speed / m);

  setChannel(2, getStickNeutral(2, 5));
  setChannel(3, getStickNeutral(3, 5));
}
