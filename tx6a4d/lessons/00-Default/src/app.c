#include "app.h"

void app() {
  setChannel(0, getStick(0));
  setChannel(1, getStick(1));
  setChannel(2, getStick(2));
  setChannel(3, getStick(3));
  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(0) && getChannel(6) < 127) {
    setChannel(6, getChannel(6) + 1);
  }

  if (getButton(1) && getChannel(6) > -127) {
    setChannel(6, getChannel(6) - 1);
  }

  if (getButton(2) && getChannel(7) < 127) {
    setChannel(7, getChannel(7) + 1);
  }

  if (getButton(3) && getChannel(7) > -127) {
    setChannel(7, getChannel(7) - 1);
  }
}
