#include "app.h"

// files in audio SD mini card
// 0001.mp3: engine start
// 0002.mp3: warning
// 0003.mp3: turning
// 0004.mp3: engine stop
// 0005.mp3: engine running

static bool isBrakeOn = false;  // 7, 8
static bool isReverseOn = false;  // 6, 9
static bool isTurnLeftON = false; // 10, 13
static bool isTurnRightON = false; // 5, 2

static bool isHeadLightOn = false;  // 0, 1, 15, 14
static bool isFlashOn = false; // 3, 4, 11, 12,

void setup() {
  neoInit(32);
}

void onPlayerReady() {
  mpVolume(30);
}

void loop() {
  static bool isSoundOn = false;

  float ratio = (getChannel(8) + 127) / 254.0;
  int8_t speed = getChannel(2) * ratio;

  if (getChannel(11)) {
    setMotor(0, -128); // brake
    isBrakeOn = true;
  } else {
    setMotor(0, speed);
    isBrakeOn = false;
  }

  setServo(0, 150 + getChannel(0) * 3 / 10 + getChannel(9) * 1 / 10);

  isReverseOn = speed < -10;

  isTurnLeftON = getChannel(0) > 10;
  isTurnRightON = getChannel(0) < -10;

  isHeadLightOn = getChannel(13);
  isFlashOn = getChannel(10);

  if (getChannel(12) && !isSoundOn) {
    mpPlay(1, true);
  }

  if (isSoundOn) {
    if (!getChannel(12)) {
      mpPlay(4, true);
    }

    if (isTurnLeftON || isTurnRightON) {
      mpPlay(3, true);  // play 0002.mp3
    }

    if (isReverseOn) {
      mpPlay(2, true);  // play 0002.mp3
    }
    mpPlay(5, false);
  }

  isSoundOn = getChannel(12);
}

void neo() {
  static uint8_t step = 0;

  uint8_t brakeBrightness = 0x00;
  uint8_t headlightBrightness = 0x00;
  uint8_t turningBrightness = 0x00;

  if (isHeadLightOn) {
    brakeBrightness = 0x08;
    turningBrightness = 0x08;
    headlightBrightness = 0x20;
  }

  neoSetColor(7, isReverseOn ? COLOR_WHITE : COLOR_RED, brakeBrightness + (isBrakeOn || isReverseOn ? 0x20 : 0));
  neoSetColor(8, isReverseOn ? COLOR_WHITE : COLOR_RED, brakeBrightness + (isBrakeOn || isReverseOn ? 0x20 : 0));
  neoSetColor(6, COLOR_RED, brakeBrightness + (isBrakeOn || isReverseOn ? 0x80 : 0x00));
  neoSetColor(9, COLOR_RED, brakeBrightness + (isBrakeOn || isReverseOn? 0x80 : 0x00));

  neoSetColor(10, COLOR_ORANGE, turningBrightness + (isTurnLeftON && (step & 0x02) ? 0x40 : 0));
  neoSetColor(13, COLOR_ORANGE, turningBrightness + (isTurnLeftON && (step & 0x02) ? 0x40 : 0));

  neoSetColor(5, COLOR_ORANGE, turningBrightness + (isTurnRightON && (step & 0x02) ? 0x40 : 0));
  neoSetColor(2, COLOR_ORANGE, turningBrightness + (isTurnRightON && (step & 0x02) ? 0x40 : 0));

  neoSetColor(0, COLOR_WHITE, headlightBrightness + (isFlashOn && headlightBrightness ? 0x80 : 0));
  neoSetColor(1, COLOR_WHITE, headlightBrightness + (isFlashOn && headlightBrightness ? 0x80 : 0));
  neoSetColor(14, COLOR_WHITE, headlightBrightness + (isFlashOn && headlightBrightness ? 0x80 : 0));
  neoSetColor(15, COLOR_WHITE, headlightBrightness + (isFlashOn && headlightBrightness ? 0x80 : 0));

  neoSetColor(3, step & 0x02 ? COLOR_BLUE : COLOR_PURPLE, isFlashOn && (step & 0x01) ? 0x80 : 0);
  neoSetColor(12, step & 0x02 ? COLOR_PURPLE : COLOR_BLUE, isFlashOn && !(step & 0x01) ? 0x80 : 0);
  neoSetColor(4, step & 0x04 ? COLOR_PURPLE : COLOR_BLUE, isFlashOn && (step & 0x01) ? 0x80 : 0);
  neoSetColor(11, step & 0x04 ? COLOR_BLUE : COLOR_PURPLE, isFlashOn && !(step & 0x01) ? 0x80 : 0);

  step++;
}
