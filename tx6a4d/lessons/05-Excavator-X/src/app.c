#include "app.h"

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

int16_t maxIn3(int16_t a, int16_t b, int16_t c) {
  int16_t m = MAX(a, b);
  return MAX(m, c);
}

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  return abs(getStick(index)) > deadzone ? getStick(index) : 0;
}

void loop() {
  int16_t left = getStickNeutral(1, 0x10) - getStickNeutral(0, 0x10);
  int16_t right = getStickNeutral(1, 0x10) + getStickNeutral(0, 0x10);

  int16_t m = maxIn3(abs(left), abs(right), 127);

  setChannel(4, right * 127. / m); // Right Motor
  setChannel(5, left * 127. / m);  // Left Motor

  setChannel(2, - getStickNeutral(2, 0x20));    // Dipper
  setChannel(3, getStickNeutral(3, 0x20) / 2);  // Swing

  setChannel(6, MIN(getStick(4), 0)); // Blade
  setChannel(7, getStick(5));         // Crawler

  if (getButton(0) == getButton(1)) { // Bucket
    setChannel(0, 0);
  } else if (getButton(1)) {
    setChannel(0, 0x60);
  } else {  // getButton(0)
    setChannel(0, -0x60);
  }

  if (getButton(2) == getButton(3)) { // Boom
    setChannel(1, 0);
  } else if (getButton(2)) {
    setChannel(1, 0x60);
  } else {  // getButton(3)
    setChannel(1, -0x60);
  }
}
