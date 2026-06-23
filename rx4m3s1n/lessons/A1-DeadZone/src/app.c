#include "app.h"

void runMotor(uint8_t index, int8_t val) {
  if (abs(val) < 8) {
    setMotor(index, 0);
  } else {
    setMotor(index, val);
  }
}

int8_t chnLast[10];

void loop() {
  uint8_t ratio = getChannel(4) + 127;

  if (getChannel(6)) {
    runMotor(1, -128);
  } else {
    runMotor(1, getChannel(1) * ratio / 300);
  }

  setServo(0, 150 - getChannel(0) * 3 / 5 + getChannel(5) / 5);

  for (uint8_t i = 0; i < 10; i++) {
    chnLast[i] = getChannel(i);
  }
}

void onDisconnect() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
  for (uint8_t i = 0; i < 3; i++) setServo(i, 150);
}
