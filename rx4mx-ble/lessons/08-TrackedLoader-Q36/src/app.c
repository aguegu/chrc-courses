#include "app.h"

// TrackedLoader for the ShanWan Q36: a tracked loader, driven by the Q36
// alone (mode switch on D, "Q36 for Android"). Its drive is 02-MiniTank's
// on the Q36's D-pad: the Q36 has no sticks and no analog triggers, so no
// tank mode and no speed knob.
//   D-pad      -> the tracks, mixed in an X (arcade): up ahead, down back,
//                 left and right turning on the spot, the diagonals curving
//   Y / A      -> the arm, DM2: held, up / down; released, it stops
//   B / X      -> the bucket, DM3: held, tipped / curled; released, it stops
//   link lost  -> every motor stopped
// Left track DM0, right track DM1 (negated for mirrored wiring, as 02's).

#define MOTOR_LEFT 0   // DM0, the left track
#define MOTOR_RIGHT 1  // DM1, the right track
#define MOTOR_ARM 2    // DM2, the arm
#define MOTOR_BUCKET 3 // DM3, the bucket
// the tracks' speed at full D-pad, of setMotor's 127: the Q36's D-pad is
// all or nothing, so this sets how fast the loader goes
#define DRIVE_SPEED 100
#define FULL 127 // a D-pad direction, as a stick pushed all the way
// the arm's and the bucket's speed while their button is held, of
// setMotor's 127; negate one to turn its motor the other way (its wiring)
// the arm reversed: this loader's arm motor is wired the other way
#define ARM_SPEED -100
#define BUCKET_SPEED 100

// a motor run by two buttons in REPORT_BUTTONS1: `plus` held, +speed;
// `minus` held, -speed; neither or both, stopped
static int8_t twoButtons(uint8_t buttons, uint8_t plus, uint8_t minus,
                         int8_t speed) {
  bool p = buttons & plus, m = buttons & minus;
  return p == m ? 0 : p ? speed : -speed;
}

// The D-pad as a stick: throttle (up +) and turn (right +), each -FULL, 0
// or FULL. From the hat, its directions clockwise from up; or, in the Q36's
// joystick mode (View and Home together), from X and Y at full deflection
// only (0, 0x80, 0xFF; X 0 left, Y 0 up)
static void dpad(int16_t *throttle, int16_t *turn) {
  static const int8_t hatThrottle[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  static const int8_t hatTurn[8] = {0, 1, 1, 1, 0, -1, -1, -1};
  uint8_t hat = getReport(REPORT_HAT) & HAT_MASK;
  if (hat < 8) {
    *throttle = hatThrottle[hat] * FULL;
    *turn = hatTurn[hat] * FULL;
    return;
  }
  uint8_t x = getReport(REPORT_X), y = getReport(REPORT_Y);
  *throttle = y < 0x40 ? FULL : y > 0xC0 ? -FULL : 0;
  *turn = x < 0x40 ? -FULL : x > 0xC0 ? FULL : 0;
}

void loop() {
  // the arcade mix (02-MiniTank): throttle and turn added on one track,
  // taken away on the other, scaled back by the larger when that passes
  // FULL, so a diagonal keeps its proportions
  int16_t throttle, turn;
  dpad(&throttle, &turn);
  int16_t left = throttle + turn;
  int16_t right = throttle - turn;
  int16_t m = abs(left) > abs(right) ? abs(left) : abs(right);
  if (m < FULL)
    m = FULL;
  setMotor(MOTOR_LEFT, left * DRIVE_SPEED / m);
  setMotor(MOTOR_RIGHT, -right * DRIVE_SPEED / m);

  // the loader: Y raises the arm, A lowers it; B tips the bucket, X curls
  // it (the buttons on the Q36's face: Y top, A bottom, B right, X left)
  uint8_t buttons = getReport(REPORT_BUTTONS1);
  setMotor(MOTOR_ARM, twoButtons(buttons, BTN1_Y, BTN1_A, ARM_SPEED));
  setMotor(MOTOR_BUCKET, twoButtons(buttons, BTN1_B, BTN1_X, BUCKET_SPEED));
}

// The pad's link lost (switched off, out of range): the loader stopped,
// tracks, arm and bucket, until the pad's next report
void onDisconnect() {
  setMotor(MOTOR_LEFT, 0);
  setMotor(MOTOR_RIGHT, 0);
  setMotor(MOTOR_ARM, 0);
  setMotor(MOTOR_BUCKET, 0);
}
