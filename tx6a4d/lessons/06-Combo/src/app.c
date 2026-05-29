#include "app.h"

#define STK_SPEED 4
#define STK_TURN_MAIN 0
#define STK_TURN_OFFSET 5
#define STK_THROTTLE 1

int8_t getStickNeutral(uint8_t index, int8_t offset, uint8_t deadzone) {
  int16_t v = getStick(index) + offset;
  if (v > 127) v = 127;
  if (v < -127) v = -127;

  if (v > deadzone) {
    return (int8_t)((v - deadzone) * 127 / (127 - deadzone));
  } else if (v < -deadzone) {
    return (int8_t)((v + deadzone) * 127 / (127 - deadzone));
  }
  return 0;
}

void loop() {
  uint16_t speed = (getStick(STK_SPEED) + 127);
  setChannel(4, - getStick(STK_TURN_MAIN) * 3 / 4 + getStick(STK_TURN_OFFSET) / 4);

  if (getButton(0)) {
    setChannel(0, -128);
  } else {
    setChannel(0, getStickNeutral(STK_THROTTLE, 0, 2) * speed / 254);
  }
}
