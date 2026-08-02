#include "app.h"

static uint8_t pixelCount = 16;

static bool ledsHeadOn = false;
static bool ledsReverseOn = false;
static bool ledsBrakeOn = false;

static bool ledsLeftOn = false;
static bool ledsRightOn = false;

static bool ledsCeilOn = false;

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default), so
// the sign-based light logic below reads like the original signed channels.
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

void setup() {
  neoSetup(pixelCount);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {
  int8_t drive = centered(1, 8); // forward / reverse throttle
  int8_t steer = centered(4, 0); // steering

  ledsHeadOn = drive < 0;
  ledsReverseOn = drive > 0;
  ledsBrakeOn = drive > getMotor(1);

  ledsLeftOn = steer > 0;
  ledsRightOn = steer < 0;

  ledsCeilOn = centered(0, 8) || centered(2, 8);

  setMotor(0, centered(0, 8));
  setMotor(1, drive);
  setMotor(2, centered(2, 8));
  setMotor(3, centered(3, 8));

  setServo(0, 150 + steer * 2 / 5);

  if (ledsLeftOn || ledsRightOn) {
    mpPlay(3, false); // play 0003.mp3
  }

  if (ledsCeilOn) {
    mpPlay(2, false); // play 0002.mp3
  }

  if (getChannel(6)) {
    mpPlay(1, false); // play 0001.mp3
  }
}

static uint8_t ledsHead[4] = {0, 1, 2, 3};
static uint8_t ledsLeft[2] = {4, 11};
static uint8_t ledsRight[2] = {7, 8};
static uint8_t ledsBrake[2] = {12, 15};
static uint8_t ledsReverse[2] = {13, 14};
static uint8_t ledsCeil[4] = {5, 6, 9, 10};

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
    neoSetColor(ledsLeft[i], COLOR_YELLOW, ledsLeftOn && (step & 0x01) ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 2; i++) {
    neoSetColor(ledsRight[i], COLOR_YELLOW, ledsRightOn && (step & 0x01) ? 0x40 : 0);
  }

  for (uint8_t i = 0; i < 4; i++) {
    neoSetColor(ledsCeil[i], COLOR_ORANGE, ledsCeilOn && ((step & 0x01) == (i & 0x01)) ? 0x20 : 0);
  }

  step++;
}
