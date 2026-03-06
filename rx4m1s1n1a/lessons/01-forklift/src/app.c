#include "app.h"

static uint8_t pixelCount = 16;

static bool ledsHeadOn = false;
static bool ledsReverseOn = false;
static bool ledsBrakeOn = false;

static bool ledsLeftOn = false;
static bool ledsRightOn = false;

static bool ledsCeilOn = false;

void setup() {
  neoInit(pixelCount);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {

  ledsHeadOn = getChannel(1) < 0;
  ledsReverseOn = getChannel(1) > 0;
  ledsBrakeOn = getChannel(1) > getMotor(1);

  ledsLeftOn = getChannel(4) > 0;
  ledsRightOn = getChannel(4) < 0;

  ledsCeilOn = getChannel(0) || getChannel(2);

  setMotor(0, getChannel(0));
  setMotor(1, getChannel(1));
  setMotor(2, getChannel(2));
  setMotor(3, getChannel(3));

  setServo(0, 150 + getChannel(4) * 2 / 5);

  if (ledsLeftOn || ledsRightOn) {
    mpPlay(3, false);  // play 0003.mp3
  }

  if (ledsCeilOn) {
    mpPlay(2, false);  // play 0002.mp3
  }

  if (getChannel(6)) {
    mpPlay(1, false);  // play 0001.mp3
  }
}

static uint8_t ledsHead[4] = { 0, 1, 2, 3 };
static uint8_t ledsLeft[2] = { 4, 11 };
static uint8_t ledsRight[2] = { 7, 8 };
static uint8_t ledsBrake[2] = { 12, 15 };
static uint8_t ledsReverse[2] = { 13, 14 };
static uint8_t ledsCeil[4] = { 5, 6, 9, 10 };

void neo() {
  static uint8_t step = 0;

  for (uint8_t i = 0; i < 4; i++) {
    neoSetColor(ledsHead[i], COLOR_WHITE, ledsHeadOn ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 2; i++) {
    neoSetColor(ledsReverse[i], COLOR_WHITE, ledsReverseOn ? 0x20 : 0);
  }

  for (uint8_t i = 0; i < 2; i++) {
    neoSetColor(ledsBrake[i], COLOR_RED, ledsBrakeOn || ledsReverseOn ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 2; i++) {
    neoSetColor(ledsBrake[i], COLOR_YELLOW, ledsLeftOn && (step & 0x01) ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 2; i++) {
    neoSetColor(ledsRight[i], COLOR_YELLOW, ledsRightOn && (step & 0x01) ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 4; i++) {
    neoSetColor(ledsCeil[i], COLOR_ORANGE, ledsCeilOn && ((step & 0x01) == (i & 0x01)) ? 0x20 : 0);
  }

  step++;
}
