#include "app.h"

static uint8_t pixelCount = 16;

void setup() {
  neoInit(pixelCount);
}

void loop() {
  setMotor(0, getChannel(0));
  setMotor(1, getChannel(1));
  setMotor(2, getChannel(2));
  setMotor(3, getChannel(3));

  setServo(0, 150 + getChannel(4) * 2 / 5);
  setServo(1, 150 + getChannel(5) * 2 / 5);
  setServo(2, 150 + getChannel(6) * 2 / 5);
}

void neo() {
  static uint8_t step = 0;

  for (uint8_t i = 0; i < pixelCount; i++) {
    if (i == step % pixelCount) {
      neoSetHSL(i, (360 / pixelCount) * ((i + step) % pixelCount), 0xff, 0x08);
    } else {
      neoSetRGB888(i, 0, 0, 0);
    }
  }

  step++;
}
