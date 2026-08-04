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
  // follow the two knobs (channels 4, 5).
  setServo(0, 150 + centered(4, 0) * 2 / 5);
  setServo(1, 150 + centered(5, 0) * 2 / 5);

  // SM2 is a button-nudged position: the four buttons step it up/down —
  // fine ±1 (btn0/btn1 = ch6/ch7), coarse ±4 (btn2/btn3 = ch8/ch9). Holding a
  // button ramps it; clamped to a wide 50..250 pulse range.
  static int16_t sm2 = 150;
  if (getChannel(6)) sm2 += 1;
  if (getChannel(7)) sm2 -= 1;
  if (getChannel(8)) sm2 += 4;
  if (getChannel(9)) sm2 -= 4;
  if (sm2 > 250) sm2 = 250;
  if (sm2 < 50) sm2 = 50;
  setServo(2, sm2);
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
