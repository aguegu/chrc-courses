#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing. The base is a 1:1
// map (0..126 -> -127..-1, 129..255 -> 1..127, center codes 127/128 -> 0),
// bounded to ±127 by construction. The deadzone widens the neutral band and
// rescales the remaining travel so a full-throw stick still reaches ±127;
// deadzone 0 leaves the pure 1:1 map.
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

void loop() {
  setMotor(0, centered(0, 8));
  setMotor(1, centered(1, 8));
  setMotor(2, centered(2, 8));
  setMotor(3, centered(3, 8));

  // Knob channels 4 and 5 each drive a mirrored servo pair around center
  // (150 = 1.5 ms): SM0/SM1 follow the knob, SM2/SM3 mirror it. Reusing
  // centered(…, 0) parks both at exactly 150 when the knob is centered.
  setServo(0, 150 + centered(4, 0) * 2 / 5);
  setServo(1, 150 + centered(5, 0) * 2 / 5);

  setServo(2, 150 - centered(4, 0) * 2 / 5);
  setServo(3, 150 - centered(5, 0) * 2 / 5);
}

// Called when the RF link drops (no packet for ~400 ms). Put the vehicle in a
// safe state: stop all motors and center every servo.
void onDisconnect() {
  for (uint8_t i = 0; i < 4; i++) setMotor(i, 0);
  for (uint8_t i = 0; i < 4; i++) setServo(i, 150);
}
