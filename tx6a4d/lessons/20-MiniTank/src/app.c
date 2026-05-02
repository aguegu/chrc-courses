#include "app.h"

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#define STK_Y_R 1
#define STK_X_R 0

#define STK_Y_L 2
#define STK_X_L 3

#define STK_L 2
#define STK_R 1

#define STK_MODE 5

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
  uint16_t speed = (getStick(STK_SPEED) + 127) / 2;

  if (getStick(STK_MODE) < -0x40) {  // X on right
    int16_t left = getStickNeutral(STK_Y_R, 5) - getStickNeutral(STK_X_R, 5);
    int16_t right = getStickNeutral(STK_Y_R, 5) + getStickNeutral(STK_X_R, 5);
    uint16_t m = maxIn3(abs(left), abs(right), 127);
    setChannel(CHN_L, + left * speed / m);
    setChannel(CHN_R, - right * speed / m);
  } else if (getStick(STK_MODE) > 0x40) { // X on left
    int16_t left = getStickNeutral(STK_Y_L, 5) - getStickNeutral(STK_X_L, 5);
    int16_t right = getStickNeutral(STK_Y_L, 5) + getStickNeutral(STK_X_L, 5);
    uint16_t m = maxIn3(abs(left), abs(right), 127);
    setChannel(CHN_L, + left * speed / m);
    setChannel(CHN_R, - right * speed / m);
  } else {  // H
    setChannel(CHN_L, + getStickNeutral(STK_L, 5) * speed / 254);
    setChannel(CHN_R, - getStickNeutral(STK_R, 5) * speed / 254);
  }
}
