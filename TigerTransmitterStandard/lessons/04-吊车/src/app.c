#include "app.h"

#ifndef MAX
#define MAX(n,m)  (((n) < (m)) ? (m) : (n))
#endif

#define GEARS       4
#define DEADZONE    10

uint8_t gear = GEARS;

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  int8_t v = getStick(index);
  if (v > deadzone) {
    return (int8_t)((float)(v - deadzone) / (127 - deadzone) * 127);
  } else if (v < -deadzone) {
    return (int8_t)((float)(v + deadzone) / (127 - deadzone) * 127);
  }
  return 0;
}

int16_t maxIn3(int16_t a, int16_t b, int16_t c) {
  return MAX(MAX(a, b), c);
}

void loop(void) {
  static bool buttonsLast[4];

  int16_t y = getStickNeutral(3, DEADZONE);
  int16_t x = getStickNeutral(2, DEADZONE);
  int16_t left  = y + x;
  int16_t right = y - x;

  int16_t m = maxIn3(abs(left), abs(right), 127);
  if (m == 0) m = 1;

  setChannel(4, (int8_t)(right * 127.0f / m * gear / GEARS));  // 右侧履带电机
  setChannel(5, (int8_t)(left  * 127.0f / m * gear / GEARS));  // 左侧履带电机

  setChannel(6, getStickNeutral(0, DEADZONE));  // 塔台转向
  setChannel(1, getStickNeutral(1, DEADZONE));  // 吊臂俯仰
  setChannel(2, getStickNeutral(4, DEADZONE));  // 吊臂伸缩
  setChannel(3, getStickNeutral(5, DEADZONE));  // 吊钩绞盘

  if (getButton(0) && !buttonsLast[0]) {
    if (gear > 1) gear--;
  }

  if (getButton(1) && !buttonsLast[1]) {
    if (gear < GEARS) gear++;
  }

  if (getButton(3) && !buttonsLast[3]) {  // LED
    if (getChannel(0)) {
      setChannel(0, 0);
    } else {
      setChannel(0, 100);
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
