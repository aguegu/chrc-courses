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
  static bool buttonsLast[4] = {false};

  int16_t y = getStickNeutral(3, DEADZONE);
  int16_t x = getStickNeutral(2, DEADZONE);
  int16_t left  = y + x;
  int16_t right = y - x;

  int16_t m = maxIn3(abs(left), abs(right), 127);
  if (m == 0) m = 1;

  setChannel(0, (int8_t)(right * 127.0f / m * gear / GEARS));  // Motor Right
  setChannel(1, (int8_t)(left  * 127.0f / m * gear / GEARS));  // Motor left

  setChannel(2, getStick(1));
  setChannel(4, getStick(0));

  if (getButton(0) && !buttonsLast[0]) {
    if (gear > 1) gear--;
  }

  if (getButton(1) && !buttonsLast[1]) {
    if (gear < GEARS) gear++;
  }

  setChannel(3, getButton(3) ? 127 : 0);  // Fire

  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
