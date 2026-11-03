#include "app.h"

// MineTruck for the ShanWan Q34P (forked from 06-Combo-Q34P): a mine truck,
// steered by a servo, its rear and front wheels on a motor each, and a
// dumper lifted by a third.
//   left stick X  -> steering, servo SM0 (reversed: the servo's linkage),
//                    STEER_RATIO % of the full swing
//   RT            -> throttle, DM0 (rear) and DM1 (front), at most DRIVE_MAX
//                    (the motor driver's protection: 240/255 of full)
//   LB            -> reverse, held: RT drives DM0 and DM1 backwards
//   RB            -> brake, DM0 and DM1 (held: over the throttle)
//   right stick Y -> the dumper, DM2: up lifts, down lowers
//   Y / A         -> the gearbox: a gear up / down a press (12/25/50/100 %)
//   View / Menu   -> SM0's centre a step left / right a press (kept)
//   link lost     -> DM0, DM1 and DM2 braked, SM0 back to its trimmed centre

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

#define SERVO_STEER 0     // SM0
#define MOTOR_REAR 0      // DM0, the rear wheels
#define MOTOR_FRONT 1     // DM1, the front wheels
#define MOTOR_DUMPER 2    // DM2, the dumper's lift
#define DUMPER_DEADZONE 4 // the right stick's rest, ignored
#define BRAKE -128        // setMotor's short brake
#define TRIM_MAX 40       // the centre's bias, at most 0.4 ms either way
// the steering's travel, in % of the stick's full swing (100: about 1.0 to
// 2.0 ms; 50: 1.25 to 1.75 ms)
#define STEER_RATIO 50
// the drive's ceiling, the motor driver's protection: 240/255 of full power
// (setMotor's 127), never passed in either direction
#define DRIVE_MAX 120

// the gearbox: the throttle's share in each gear, in %; the first at start
static const uint8_t gears[] = {12, 25, 50, 100};
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
  // the same to the rear and the front wheels
  int16_t throttle = getReport(REPORT_RT) * DRIVE_MAX / 255;
  throttle = throttle * gears[gear] / 100;
  int8_t drive;
  if (buttons & BTN1_RB)
    drive = BRAKE;
  else if (buttons & BTN1_LB)
    drive = -throttle;
  else
    drive = throttle;
  setMotor(MOTOR_REAR, drive);
  setMotor(MOTOR_FRONT, drive);

  // the dumper: the right stick's Y (0 up, so negated: up lifts), up to
  // DRIVE_MAX either way; no gears
  setMotor(MOTOR_DUMPER,
           -centered(REPORT_RZ, DUMPER_DEADZONE) * DRIVE_MAX / 127);
}

// The pad's link lost (switched off, or out of range: nothing heard for the
// link's supervision timeout): the truck braked, its dumper held where it is
// (braked, not let down by its load) and its wheels straight, until the
// pad's next report runs loop() again
void onDisconnect() {
  setMotor(MOTOR_REAR, BRAKE);
  setMotor(MOTOR_FRONT, BRAKE);
  setMotor(MOTOR_DUMPER, BRAKE);
  setServo(SERVO_STEER, 150 - trim);
}
