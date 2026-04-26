#include "app.h"

#define STK_ENGINE 2
#define STK_LIFT 1
#define STK_TURN 0
#define STK_PITCH 3

#define STK_THROTTLE 4
#define STK_BIAS 5

#define BTN_ENGINE_BRAKE 1
#define BTN_LIFT_BRAKE 2
#define BTN_LED 3

#define CHN_TURN 4
#define CHN_ENGINE 3
#define CHN_LIFT 2
#define CHN_PITCH 1
#define CHN_LED 0

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  int8_t v = getStick(index);
  if (v > deadzone) {
    return (float)(v - deadzone) / (127 - deadzone) * 127;
  } else if (v < -deadzone) {
    return (float)(v + deadzone) / (127 - deadzone) * 127;
  }
  return 0;
}

int8_t stickToMotor(uint8_t index, uint8_t deadzone) {
  double f = getStickNeutral(index, deadzone) / 127.0;
  return (int8_t)(f * f * 127.0 * (f > 0 ? 1 : -1));
}

void loop() {
  uint8_t throttle = getStick(STK_THROTTLE) + 127;

  setChannel(CHN_PITCH, stickToMotor(STK_PITCH, 0x20) / 4);

  if (getButton(BTN_ENGINE_BRAKE)) {
    setChannel(CHN_ENGINE, -128);
  } else {
    setChannel(CHN_ENGINE, - stickToMotor(STK_ENGINE, 5) * throttle / 254);
  }

  if (getButton(BTN_LIFT_BRAKE)) {
    setChannel(CHN_LIFT, -128);
  } else {
    setChannel(CHN_LIFT, stickToMotor(STK_LIFT, 5) / 4);
  }

  setChannel(CHN_TURN, getStickNeutral(STK_TURN, 5) * 2 / 3 + getStick(STK_BIAS) / 3);

  setChannel(CHN_LED, getButton(BTN_LED) ? 0x40 : 0);
}
