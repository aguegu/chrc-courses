#include "app.h"

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#define DEADZONE (5)

int8_t getStickNeutral(uint8_t index) {
  return ABS(getStick(index)) > DEADZONE ? getStick(index) : 0;
}

void app() {
  setChannel(0, getStickNeutral(2));
  setChannel(1, getStickNeutral(3));
  setChannel(2, getStickNeutral(0));
  setChannel(3, getStickNeutral(1));

  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(2) == getButton(3)) {
    setChannel(6, 0);
  } else if (getButton(2)) {
    setChannel(6, 127);
  } else {  // getButton(3)
    setChannel(6, -127);
  }

  if (getButton(1) == getButton(0)) {
    setChannel(7, 0);
  } else if (getButton(1)) {
    setChannel(7, 127);
  } else {  // getButton(0)
    setChannel(7, -127);
  }
}
