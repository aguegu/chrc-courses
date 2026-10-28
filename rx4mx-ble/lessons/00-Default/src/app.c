#include "app.h"
#include <string.h>

// The default for a Bluetooth gamepad: it reads the pad's report by its
// bytes and button masks (app.h's REPORT_ and BTN names).

// Center a raw 0..255 stick to a signed -127..+127 swing. The base is a 1:1
// map (0..126 -> -127..-1, 129..255 -> 1..127, center codes 127/128 -> 0),
// bounded to ±127 by construction. The deadzone widens the neutral band and
// rescales the remaining travel so a full-throw stick still reaches ±127;
// deadzone 0 leaves the pure 1:1 map.
static int8_t centered(uint8_t byte, uint8_t deadzone) {
  int16_t r = getReport(byte);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone)
    return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone)
    return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

#define MOTOR_LEFT 0  // DM0
#define MOTOR_RIGHT 1 // DM1
#define BRAKE -128    // setMotor's short brake

// The pad linked, by the name it gives (the screen's row 2): a code kept for
// loop(). The ShanWan Q36 has no sticks, so it drives with its D-pad (its
// joystick mode would give the same eight directions); any other pad with
// its left stick
#define PAD_OTHER 0
#define PAD_Q36 1
static uint8_t pad;

void onPaired(const char *name) {
  pad = strcmp(name, "Q36 for Android") == 0 ? PAD_Q36 : PAD_OTHER;
}

void loop() {
  // DM0 and DM1 driven by throttle and turn, mixed in an X (arcade, as
  // chrc-courses' 02-MiniTank): ahead, both forward; to the side, they turn
  // the vehicle. From the left stick (its Y is 0 up, so the throttle is its
  // negative), or the Q36's D-pad. The mix is scaled back when it would
  // pass full speed, keeping its proportions; the right motor negated for
  // mirrored wiring, as the lesson's.
  int16_t throttle, turn;
  if (pad == PAD_Q36) {
    // the D-pad as a stick at full deflection, its directions clockwise
    // from up; at rest, nothing
    throttle = 0;
    turn = 0;
    switch (getReport(REPORT_HAT) & HAT_MASK) {
    case 0: // up
      throttle = 127;
      break;
    case 1: // up-right
      throttle = 127;
      turn = 127;
      break;
    case 2: // right
      turn = 127;
      break;
    case 3: // down-right
      throttle = -127;
      turn = 127;
      break;
    case 4: // down
      throttle = -127;
      break;
    case 5: // down-left
      throttle = -127;
      turn = -127;
      break;
    case 6: // left
      turn = -127;
      break;
    case 7: // up-left
      throttle = 127;
      turn = -127;
      break;
    }
  } else {
    throttle = -centered(REPORT_Y, 8);
    turn = centered(REPORT_X, 8);
  }
  int16_t left = throttle + turn;
  int16_t right = throttle - turn;
  int16_t m = abs(left) > abs(right) ? abs(left) : abs(right);
  if (m < 127)
    m = 127;
  if (getReport(REPORT_BUTTONS1) & BTN1_LB) { // LB held: brake, over the stick
    setMotor(MOTOR_LEFT, BRAKE);
    setMotor(MOTOR_RIGHT, BRAKE);
  } else {
    setMotor(MOTOR_LEFT, left * 127 / m);
    setMotor(MOTOR_RIGHT, -right * 127 / m);
  }
}

// The pad's link lost (switched off, out of range): the vehicle stopped,
// as a released pad would leave it, until its next report
void onDisconnect() {
  setMotor(MOTOR_LEFT, 0);
  setMotor(MOTOR_RIGHT, 0);
}
