#include "app.h"

#define STK_L 2
#define STK_R 1
#define STK_SPEED 4

#define CHN_L 0
#define CHN_R 1

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
  uint16_t speed = getStick(STK_SPEED) + 127;

  setChannel(CHN_L, getStickNeutral(STK_L, 5) * speed / 254);
  setChannel(CHN_R, getStickNeutral(STK_R, 5) * speed / 254);

  setChannel(2, getStickNeutral(0, 5));
  setChannel(3, getStickNeutral(3, 5));
}
