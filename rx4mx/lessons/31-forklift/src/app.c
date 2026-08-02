#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

// Squared stick response — gentle near center, full power at the ends.
static int8_t stickToMotor(uint8_t index, uint8_t deadzone) {
  float f = centered(index, deadzone) / 127.0f;
  return (int8_t)(f * f * 127.0f * (f > 0 ? 1 : -1));
}

// Lighting state, derived each loop() from how the truck is being driven.
static bool headOn, brakeOn, reverseOn, leftOn, rightOn, ceilOn;

void setup() {
  neoSetup(16);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {
  uint16_t throttle = getChannel(4); // left knob: speed limit 0..255
  bool engineBrake = getChannel(7);  // button 1

  // drive: fwd/back on left stick Y, speed-limited; engine-brake forces stop
  int8_t drive = engineBrake ? -128 : -stickToMotor(2, 5) * throttle / 255;
  setMotor(0, drive);

  // steering: right stick X + bias knob (ch5) -> servo SM0
  int8_t steer = centered(0, 5) * 2 / 3 + centered(5, 0) / 3;
  setServo(0, 150 + steer * 2 / 5);

  // arm: pitch (left stick X), lift (right stick Y) with a lift brake
  setMotor(2, stickToMotor(3, 0x20) / 4);
  int8_t lift = getChannel(8) ? -128 : stickToMotor(1, 5) / 4;
  setMotor(1, lift);

  // --- lighting, from the driving state ---
  headOn = getChannel(9);      // headlights toggle (button 3)
  reverseOn = drive > 0;       // driving in reverse -> reverse lamps
  brakeOn = engineBrake || reverseOn;
  leftOn = steer > 20;         // steering -> turn signals
  rightOn = steer < -20;
  ceilOn = lift != 0;          // amber beacon while working the fork

  // sound: event cues interrupt (force), the engine loop fills the gaps.
  if (leftOn || rightOn) mpPlay(3, true); // 0003.mp3 turn signal
  if (lift != 0)         mpPlay(2, true); // 0002.mp3 fork / hydraulics
  mpPlay(5, false);                       // 0005.mp3 engine idle loop
}

// LED groups on the 16-pixel ring.
static const uint8_t ledsHead[4] = {0, 1, 2, 3};
static const uint8_t ledsLeft[2] = {4, 11};
static const uint8_t ledsRight[2] = {7, 8};
static const uint8_t ledsBrake[2] = {12, 15};
static const uint8_t ledsReverse[2] = {13, 14};
static const uint8_t ledsCeil[4] = {5, 6, 9, 10};

void neo() {
  static uint8_t step = 0;

  for (uint8_t i = 0; i < 4; i++)
    neoSetColor(ledsHead[i], COLOR_WHITE, headOn ? 0x40 : 0);

  for (uint8_t i = 0; i < 2; i++)
    neoSetColor(ledsReverse[i], COLOR_WHITE, reverseOn ? 0x20 : 0);

  for (uint8_t i = 0; i < 2; i++)
    neoSetColor(ledsBrake[i], COLOR_RED, brakeOn ? 0x60 : 0);

  for (uint8_t i = 0; i < 2; i++)
    neoSetColor(ledsLeft[i], COLOR_YELLOW, leftOn && (step & 0x02) ? 0x40 : 0);

  for (uint8_t i = 0; i < 2; i++)
    neoSetColor(ledsRight[i], COLOR_YELLOW, rightOn && (step & 0x02) ? 0x40 : 0);

  for (uint8_t i = 0; i < 4; i++)
    neoSetColor(ledsCeil[i], COLOR_ORANGE, ceilOn && (step & 0x02) ? 0x20 : 0);

  step++;
}
