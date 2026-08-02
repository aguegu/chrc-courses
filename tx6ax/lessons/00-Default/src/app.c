#include "app.h"

void loop() {
  setChannel(0, getStick(0));
  setChannel(1, getStick(1));
  setChannel(2, getStick(2));
  setChannel(3, getStick(3));
  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  setChannel(6, getButton(0));
  setChannel(7, getButton(1));
  setChannel(8, getButton(2));
  setChannel(9, getButton(3));
}
