#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

static uint8_t pixelCount = 16;

void setup() {
  neoSetup(pixelCount);
}

void loop() {
  setMotor(0, centered(0, 8));
  setMotor(1, centered(1, 8));
  setMotor(2, centered(2, 8));
  setMotor(3, centered(3, 8));

  // With NEO_ON_SM3, SM3 drives the LED strip; SM0..SM2 stay servos. SM0/SM1
  // follow the two knobs (channels 4, 5); SM2 is a two-position servo driven
  // by button 0 (channel 6): 100 (1.0 ms) released, 200 (2.0 ms) pressed.
  setServo(0, 150 + centered(4, 0) * 2 / 5);
  setServo(1, 150 + centered(5, 0) * 2 / 5);
  setServo(2, getChannel(6) ? 200 : 100);
}

// Called ~8 times/second. A single lit pixel walks around the ring, its hue
// shifting as it goes; all other pixels are off.
void neo() {
  static uint8_t step = 0;

  for (uint8_t i = 0; i < pixelCount; i++) {
    if (i == step % pixelCount) {
      neoSetHSL(i, (360 / pixelCount) * ((i + step) % pixelCount), 0xff, 0x08);
    } else {
      neoSetColor(i, 0, 0); // off (lightness 0)
    }
  }

  step++;
}
