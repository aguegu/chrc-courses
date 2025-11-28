#include "app.h"

void loop() {
  setChannel(0, getStick(2));
  setChannel(1, getStick(3));
  setChannel(2, getStick(0));
  setChannel(3, getStick(1));

  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(0) == getButton(1)) {
    setChannel(6, 0);
  } else if (getButton(1)) {
    setChannel(6, 127);
  } else {  // getButton(0)
    setChannel(6, -127);
  }

  if (getButton(2) == getButton(3)) {
    setChannel(7, 0);
  } else if (getButton(2)) {
    setChannel(7, 127);
  } else {  // getButton(3)
    setChannel(7, -127);
  }
}
