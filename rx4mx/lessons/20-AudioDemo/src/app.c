#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

// Set the MP3 volume once the module has finished powering up (~1 s).
void onPlayerReady() {
  mpVolume(15);
}

static uint8_t chnLast[10];

void loop() {
  static bool isSoundOn = false;

  // 4 motors with a deadzone of 10; SM0 and SM3 are servos (SM1+SM2 carry
  // the audio module, so setServo(1)/setServo(2) are no-ops here).
  setMotor(0, centered(0, 10));
  setMotor(1, centered(1, 10));
  setMotor(2, centered(2, 10));
  setMotor(3, centered(3, 10));

  setServo(0, 150 + centered(4, 0) * 2 / 5);
  setServo(3, 150 + centered(5, 0) * 2 / 5);

  // Buttons arrive on channels 6..9 (tx6ax). Button 3 (ch9) toggles audio on
  // a rising edge; while on, buttons 0/1/2 (ch6/7/8) fire one-shots over a
  // looping background track.
  if (!chnLast[9] && getChannel(9)) {
    isSoundOn = !isSoundOn;
    if (!isSoundOn) mpStop();
  }

  if (isSoundOn) {
    if (getChannel(6)) mpPlay(2, true); // 0002.mp3
    if (getChannel(7)) mpPlay(3, true); // 0003.mp3
    if (getChannel(8)) mpPlay(4, true); // 0004.mp3
    mpPlay(5, false);                   // 0005.mp3 background
  }

  for (uint8_t i = 0; i < 10; i++) chnLast[i] = getChannel(i);
}

void onDisconnect() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
  setServo(0, 150);
  setServo(3, 150);
}
