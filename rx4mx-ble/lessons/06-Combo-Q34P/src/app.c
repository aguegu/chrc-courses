#include "app.h"

// Combo for the ShanWan Q34P: a car, steered by a servo and driven
// by one motor.
//   left stick X -> steering, servo SM0 (reversed: the servo's linkage),
//                   STEER_RATIO % of the full swing
//   RT           -> throttle, motor DM0, at most DRIVE_MAX (the driver's
//                   protection: 240/255 of full)
//   LB           -> reverse, held: RT drives DM0 backwards
//   RB           -> brake, DM0 (held: over the throttle)
//   Y / A        -> the gearbox: a gear up / down a press (25/50/75/100 %)
//   View / Menu  -> SM0's centre a step left / right a press (kept)
//   link lost    -> DM0 braked, SM0 back to its (trimmed) centre

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
// the steering's travel, in % of the stick's full swing (100: about 1.0 to
// 2.0 ms; 50: 1.25 to 1.75 ms)
#define STEER_RATIO 50
// the drive's ceiling, the motor driver's protection: 240/255 of full power
// (setMotor's 127), never passed in either direction
#define DRIVE_MAX 120

// the gearbox: the throttle's share in each gear, in %; the first at start
static const uint8_t gears[] = {25, 50, 75, 100};
#define GEARS (sizeof gears / sizeof gears[0])

static int8_t trim;          // the centre's bias, in steps of 0.01 ms
static uint8_t buttons2Last; // REPORT_BUTTONS2 at the last report
static uint8_t gear;         // gears[]' index, kept through a lost link
static uint8_t buttons1Last; // REPORT_BUTTONS1 at the last report

void loop() {
  // steering: 1.5 ms centred, reversed (the stick's X is 0 at the left):
  // at STEER_RATIO 100 about 2.0 ms full left to 1.0 ms full right
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
  setServo(SERVO_STEER, 150 - (steer * 2 * STEER_RATIO / 500 + trim));

  // the gearbox: Y a gear up, A a gear down, once a press (its edge)
  uint8_t buttons = getReport(REPORT_BUTTONS1);
  uint8_t shifted = buttons & ~buttons1Last;
  buttons1Last = buttons;
  if ((shifted & BTN1_Y) && gear < GEARS - 1)
    gear++;
  if ((shifted & BTN1_A) && gear > 0)
    gear--;

  // RB held brakes; else the throttle: RT released 0 (stopped) .. pulled
  // all the way DRIVE_MAX, times the gear's share, ahead, or astern with LB
  // held
  int16_t throttle = getReport(REPORT_RT) * DRIVE_MAX / 255;
  throttle = throttle * gears[gear] / 100;
  if (buttons & BTN1_RB)
    setMotor(MOTOR_DRIVE, BRAKE);
  else if (buttons & BTN1_LB)
    setMotor(MOTOR_DRIVE, -throttle);
  else
    setMotor(MOTOR_DRIVE, throttle);
}

// The pad's link lost (switched off, or out of range: nothing heard for the
// link's supervision timeout): the car braked and its wheels straight, until
// the pad's next report runs loop() again
void onDisconnect() {
  setMotor(MOTOR_DRIVE, BRAKE);
  setServo(SERVO_STEER, 150 - trim);
}
