#include "app.h"

// Combo for the ShanWan Q34P: a car, steered by a servo and driven
// by one motor.
//   left stick X -> steering, servo SM0 (reversed: the servo's linkage)
//   RT           -> throttle, motor DM0
//   LB           -> reverse, held: RT drives DM0 backwards
//   RB           -> brake, DM0 (held: over the throttle)
//   View / Menu  -> SM0's centre a step left / right a press (kept)

// Center a raw 0..255 stick to a signed -127..+127 swing (00-Default's).
static int8_t centered(uint8_t byte, uint8_t deadzone) {
  int16_t r = getReport(byte);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone)
    return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone)
    return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

#define SERVO_STEER 0 // SM0
#define MOTOR_DRIVE 0 // DM0
#define BRAKE -128    // setMotor's short brake
#define TRIM_MAX 40   // the centre's bias, at most 0.4 ms either way

static int8_t trim;           // the centre's bias, in steps of 0.01 ms
static uint8_t buttons2Last;  // REPORT_BUTTONS2 at the last report

void loop() {
  // steering: 1.5 ms centred, reversed: about 2.0 ms full left to 1.0 ms
  // full right (the stick's X is 0 at the left)
  // View and Menu step the centre once a press (on its edge), in the
  // stick's sense: View as a nudge to the left
  uint8_t buttons2 = getReport(REPORT_BUTTONS2);
  uint8_t pressed = buttons2 & ~buttons2Last;
  buttons2Last = buttons2;
  if ((pressed & BTN2_VIEW) && trim > -TRIM_MAX)
    trim--;
  if ((pressed & BTN2_MENU) && trim < TRIM_MAX)
    trim++;

  int16_t steer = centered(REPORT_X, 0);
  setServo(SERVO_STEER, 150 - (steer * 2 / 5 + trim));

  // RB held brakes; else the throttle: RT released 0 (stopped) .. pulled
  // 255 (full), ahead, or astern with LB held
  uint8_t buttons = getReport(REPORT_BUTTONS1);
  int16_t throttle = getReport(REPORT_RT) / 2;
  if (buttons & BTN1_RB)
    setMotor(MOTOR_DRIVE, BRAKE);
  else if (buttons & BTN1_LB)
    setMotor(MOTOR_DRIVE, -throttle);
  else
    setMotor(MOTOR_DRIVE, throttle);
}
