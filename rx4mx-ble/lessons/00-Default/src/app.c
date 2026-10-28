#include "app.h"

// Center a raw 0..255 byte to a signed -127..+127 swing. The base is a 1:1
// map (0..126 -> -127..-1, 129..255 -> 1..127, center codes 127/128 -> 0),
// bounded to ±127 by construction. The deadzone widens the neutral band and
// rescales the remaining travel so a full-throw stick still reaches ±127;
// deadzone 0 leaves the pure 1:1 map.
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getReport(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

void loop() {
  // The sticks in HID's directions read 0 to the left and up, so centered()
  // is positive to the right and down; negated, positive is left and up, as
  // in rx4mx's lessons.
  setMotor(0, -centered(REPORT_Z, 8));  // right stick X
  setMotor(1, -centered(REPORT_RZ, 8)); // right stick Y
  setMotor(2, -centered(REPORT_Y, 8));  // left stick Y
  setMotor(3, -centered(REPORT_X, 8));  // left stick X

  // The knobs (LT: left, RT: right) each drive a mirrored servo pair around
  // center (150 = 1.5 ms): SM0/SM1 follow the knob, SM2/SM3 mirror it.
  // Reusing centered(…, 0) parks both at exactly 150 when the knob is centered.
  setServo(0, 150 + centered(REPORT_LT, 0) * 2 / 5);
  setServo(1, 150 + centered(REPORT_RT, 0) * 2 / 5);

  setServo(2, 150 - centered(REPORT_LT, 0) * 2 / 5);
  setServo(3, 150 - centered(REPORT_RT, 0) * 2 / 5);
}
