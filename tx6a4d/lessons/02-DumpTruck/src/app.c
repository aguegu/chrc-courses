#include "app.h"

#define STK_ENGINE 1
#define STK_TURN 0
#define STK_LIFT 2

#define STK_THROTTLE 4
#define STK_BIAS 5

#define CHN_ENGINE 1
#define CHN_TURN 4
#define CHN_LIFT 3

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
  uint8_t throttle = getStick(STK_THROTTLE) + 127;

  setChannel(CHN_ENGINE, - getStickNeutral(STK_ENGINE, 5) * throttle / 254);
  setChannel(CHN_LIFT, getStickNeutral(STK_LIFT, 0x20));

  setChannel(CHN_TURN, getStickNeutral(STK_TURN, 5) * 3 / 4 + getStick(STK_BIAS) / 4);
}
