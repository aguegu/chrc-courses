#include "app.h"

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  return ABS(getStick(index)) > deadzone ? getStick(index) : 0;
}

void app() {
  setChannel(0, getStickNeutral(3, 5) / 2);
  setChannel(1, - getStickNeutral(2, 5));
  setChannel(2, getStickNeutral(1, 5));

  setChannel(3, getStick(4)); // led
  
  setChannel(4, - getStickNeutral(0, 5));

}
