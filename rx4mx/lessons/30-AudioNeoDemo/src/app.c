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

// Set volume and play a boot jingle (0001.mp3) once the module is ready.
void onPlayerReady() {
  mpVolume(10);
  mpPlay(1, false);
}

void loop() {
  setMotor(0, centered(0, 8));
  setMotor(1, centered(1, 8));
  setMotor(2, centered(2, 8));
  setMotor(3, centered(3, 8));

  // Only SM0 is a servo here (SM1+SM2 = audio, SM3 = neo).
  setServo(0, 150 + centered(4, 0) * 2 / 5);
}

// A rainbow that rotates around the ring, with a bright white cursor.
void neo() {
  static uint8_t step = 0;

  for (uint8_t i = 0; i < pixelCount; i++) {
    neoSetHSL(i, (360 / pixelCount) * ((i + step) % pixelCount), 0xff, 0x08);
  }

  neoSetColor(step % pixelCount, COLOR_WHITE, 0x40);

  step++;
}
