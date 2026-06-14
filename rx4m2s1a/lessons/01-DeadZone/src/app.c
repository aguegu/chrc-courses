#include "app.h"

void onPlayerReady() {
  mpVolume(15);
}

void runMotor(uint8_t index, int8_t val) {
  if (abs(val) < 10) {
    setMotor(index, 0);
  } else {
    setMotor(index, val);
  }
}

int8_t chnLast[10];

void loop() {
  static bool isSoundOn = false;

  runMotor(0, getChannel(0));
  runMotor(1, getChannel(1));
  runMotor(2, getChannel(2));
  runMotor(3, getChannel(3));

  setServo(0, 150 + getChannel(4) * 2 / 5);
  setServo(3, 150 + getChannel(5) * 2 / 5);

  if (!chnLast[9] && getChannel(9)) {
    isSoundOn = !isSoundOn;
    if (!isSoundOn) {
      mpStop();
    }
  }

  if (isSoundOn) {
    if (getChannel(6)) {
      mpPlay(2, true);  // play 0002.mp3
    }

    if (getChannel(7)) {
      mpPlay(3, true);  // play 0003.mp3
    }

    if (getChannel(8)) {
      mpPlay(4, true);  // play 0004.mp3
    }

    mpPlay(5, false);   // play 0005.mp3 background
  }

  for (uint8_t i = 0; i < 10; i++) {
    chnLast[i] = getChannel(i);
  }
}

void onDisconnect() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
  setServo(0, 150);
  setServo(3, 150);
}
