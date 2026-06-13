#include "app.h"

void loop() {
  setMotor(0, getChannel(0));
  setMotor(1, getChannel(1));
  setMotor(2, getChannel(2));
  setMotor(3, getChannel(3));

  setServo(0, 150 + getChannel(4) * 2 / 5);
  setServo(3, 150 + getChannel(5) * 2 / 5);
}

void onDisconnect() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
  setServo(0, 150);
  setServo(3, 150);
}
