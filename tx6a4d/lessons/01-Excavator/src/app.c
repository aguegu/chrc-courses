#include "app.h"

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  return ABS(getStick(index)) > deadzone ? getStick(index) : 0;
}

void loop() {
  setChannel(0, getStickNeutral(0, 0x20));    // Bucket
  setChannel(1, - getStickNeutral(1, 0x20));  // Boom
  setChannel(2, - getStickNeutral(2, 0x20));  // Dipper
  setChannel(3, getStickNeutral(3, 0x20) / 2);    // Swing

  setChannel(6, MIN(getStick(4), 0)); // Blade
  setChannel(7, getStick(5)); // Crawler

  if (getButton(0) == getButton(1)) { // Left Motor
    setChannel(5, 0);
  } else if (getButton(1)) {
    setChannel(5, 0x60);
  } else {  // getButton(0)
    setChannel(5, -0x60);
  }

  if (getButton(2) == getButton(3)) { // Right Motor
    setChannel(4, 0);
  } else if (getButton(2)) {
    setChannel(4, 0x60);
  } else {  // getButton(3)
    setChannel(4, -0x60);
  }
}
