#include "app.h"

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  return ABS(getStick(index)) > deadzone ? getStick(index) : 0;
}

void loop() {
  setChannel(0, getStickNeutral(0, 4));
  setChannel(1, getStickNeutral(1, 4));
  setChannel(2, getStick(2)); // no spring in STK[2]
  setChannel(3, getStickNeutral(3, 4));

  setChannel(8, getStick(4));
  setChannel(9, getStick(5));

  setChannel(10, getButton(0));
  setChannel(11, getButton(1));
  setChannel(12, getButton(2));
  setChannel(13, getButton(3));
}
