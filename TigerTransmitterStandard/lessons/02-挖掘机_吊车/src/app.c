#include "app.h"

int8_t getStickNeutral(uint8_t index, uint8_t deadzone) {
  int8_t v = getStick(index);
  if (v > deadzone) {
    return (float)(v - deadzone) / (127 - deadzone) * 127;
  } else if (v < -deadzone) {
    return (float)(v + deadzone) / (127 - deadzone) * 127;
  }
  return 0;
}
void loop() {
  setChannel(0, getStickNeutral(0, 20));      // Bucket
  setChannel(1, - getStickNeutral(1, 20));    // Boom
  setChannel(2, - getStickNeutral(2, 20));    // Dipper
  setChannel(3, getStickNeutral(3,20) );  // Swing

  setChannel(6, getStick(4)); // Blade
  setChannel(7, getStick(5));         // Crawler

  if (getButton(2) == getButton(3)) { // Right Motor
    setChannel(4, 0);
  } else if (getButton(2)) {
    setChannel(4, 127);
  } else {  // getButton(3)
    setChannel(4, -127);
  }

  if (getButton(0) == getButton(1)) { // Left Motor
    setChannel(5, 0);
  } else if (getButton(1)) {
    setChannel(5, 127);
  } else {  // getButton(0)
    setChannel(5, -127);
  }
}
