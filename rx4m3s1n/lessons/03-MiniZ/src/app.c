#include "app.h"

void setup() {
  neoInit(4);
}

void loop() {
  float ratio = (getChannel(8) + 127) / 254.0;
  int8_t speed = getChannel(2) * ratio;

  if (getChannel(11) || getChannel(12)) {
    setMotor(0, -128); // brake
  } else {
    setMotor(0, -speed);
  }

  setServo(0, 150 - getChannel(0) * 2 / 10 - getChannel(9) * 1 / 10);
}
