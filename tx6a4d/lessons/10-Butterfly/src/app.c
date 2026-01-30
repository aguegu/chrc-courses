#include "app.h"

void loop() {
  static bool buttonsLast[4] = { false, false, false, false };

  setChannel(0, getStick(0));
  setChannel(1, getStick(1));
  setChannel(2, getStick(2));
  setChannel(3, getStick(3));
  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(0) && !buttonsLast[0]) {
    if (getChannel(6) < 127) {
      setChannel(6, getChannel(6) + 1);
    }
  }

  if (getButton(1) && !buttonsLast[1]) {
    if (getChannel(6) > -127) {
      setChannel(6, getChannel(6) - 1);
    }
  }

  if (getButton(2) && !buttonsLast[2]) {
    if (getChannel(7) < 127) {
      setChannel(7, getChannel(7) + 1);
    }
  }

  if (getButton(3) && !buttonsLast[3]) {
    if (getChannel(7) > -127) {
      setChannel(7, getChannel(7) - 1);
    }
  }

  for (uint8_t i = 0; i < 4; i++) {
    buttonsLast[i] = getButton(i);
  }
}
