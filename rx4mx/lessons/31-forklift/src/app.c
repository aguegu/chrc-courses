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

static bool lightsOn = false;

void setup() {
  neoSetup(16);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {
  uint16_t throttle = getChannel(4); // left knob: speed limit 0..255

  // --- drive + steering: the 03-Combo base ---
  if (getChannel(7)) {               // engine brake (button 1)
    setMotor(0, -128);
  } else {                           // fwd/back on left stick Y, speed-limited
    setMotor(0, -stickToMotor(2, 5) * throttle / 254);
  }

  // steering: right stick X + bias knob (ch5) -> servo SM0
  int16_t steer = centered(0, 5) * 2 / 3 + centered(5, 0) / 3;
  setServo(0, 150 + steer * 2 / 5);

  // --- forklift arm ---
  setMotor(2, stickToMotor(3, 0x20) / 4); // pitch on left stick X

  if (getChannel(8)) {               // lift brake (button 2)
    setMotor(1, -128);
  } else {                           // lift on right stick Y
    setMotor(1, stickToMotor(1, 5) / 4);
  }

  lightsOn = getChannel(9);          // headlights (button 3)

  mpPlay(5, false);                  // engine-running loop (0005.mp3)
}

// Headlights: white strip when the light button is on.
void neo() {
  for (uint8_t i = 0; i < 16; i++) {
    neoSetColor(i, COLOR_WHITE, lightsOn ? 0x40 : 0);
  }
}
