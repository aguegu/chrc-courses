#include "app.h"

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  return ABS(getStick(index)) > deadzone ? getStick(index) : 0;
}

void loop() {
  setChannel(0, getStickNeutral(3, 5) / 2); // Pitch
  setChannel(1, - getStickNeutral(2, 5) / 2); // Engine
  setChannel(2, getStickNeutral(1, 5)); // Lift

  setChannel(3, - getStick(4)); // Led

  setChannel(4, - getStickNeutral(0, 5) / 2); // Turn

}
