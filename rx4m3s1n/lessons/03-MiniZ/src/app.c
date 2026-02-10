#include "app.h"

void setup() {
  neoInit(4);
}

void loop() {
  int8_t speed = (getChannel(2) + 127) / 2;

  if (getChannel(10)) {
    speed = -speed;
  }

  if (getChannel(11)) {
    setMotor(0, -128); // brake
  } else {
    setMotor(0, - speed);
  }

  setServo(0, 150 - getChannel(0) * 3 / 10 - getChannel(9) * 1 / 10);
}
