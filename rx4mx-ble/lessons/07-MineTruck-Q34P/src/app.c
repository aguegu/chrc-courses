#include "app.h"

// MineTruck for the ShanWan Q34P (forked from 06-Combo-Q34P): a mine truck,
// steered by a servo, its rear and front wheels on a motor each, and a
// dumper lifted by a third; DM3 for an optional LED group, X its mode.
//   left stick X  -> steering, servo SM0 (reversed: the servo's linkage),
//                    STEER_RATIO % of the full swing
//   RT            -> throttle, DM0 (rear) and DM1 (front), at most DRIVE_MAX
//                    (the motor driver's protection: 240/255 of full)
//   LB            -> the direction, a press flips it: forward / reverse
//   RB            -> brake, DM0 and DM1 (held: over the throttle)
//   right stick Y -> the dumper, DM2: up lifts, down lowers
//   X             -> the LEDs on DM3, a mode a press: off, on, blinking
//                    (2 Hz, timed by tick()), round again
//   Y / A         -> the gearbox: a gear up / down a press (12/25/50/100 %)
//   View / Menu   -> SM0's centre a step left / right a press (kept)
//   link lost     -> DM0 to DM2 stopped, the LEDs blinking fast (every 4th
//                    tick) until the pad is back, SM0 back to its trimmed
//                    centre

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
#define LEDS 3            // DM3: an optional LED group, on its + side
#define LED_BRIGHTNESS 64 // DM3 when lit: about half duty (up to DRIVE_MAX)
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
static bool reverse;         // the direction LB set; forward at start

// the LEDs' modes on DM3, one a press of X, round and round; off at start
#define LED_OFF_MODE 0
#define LED_ON_MODE 1
#define LED_BLINK_MODE 2
#define LED_MODES 3
static uint8_t ledMode;
static bool blinkLit; // the blinking's phase, flipped every 8th tick
static bool linkLost; // onDisconnect() till the pad's next report: fast blink
static bool lostLit;  // the fast blinking's phase, flipped every 4th tick

// DM3 as the LEDs' mode says, or the fast blinking while the link is lost
static void ledsShow(void) {
  bool lit = linkLost ? lostLit
                      : ledMode == LED_ON_MODE ||
                            (ledMode == LED_BLINK_MODE && blinkLit);
  setMotor(LEDS, lit ? LED_BRIGHTNESS : 0);
}

// The scaffold's clock, 32 times a second whether the pad sends reports or
// not (the Q34P sends none at rest, so loop() alone can't keep time): the
// blinking flips every 8th tick (250 ms lit, 250 ms dark: 2 Hz), the link
// lost's every 4th (125 ms: twice as fast)
void tick() {
  static uint8_t ticks;
  ++ticks;
  if (ticks % 8 == 0)
    blinkLit = !blinkLit;
  if (ticks % 4 == 0)
    lostLit = !lostLit;
  ledsShow();
}

void loop() {
  linkLost = false; // a report: the pad is back, the LEDs as X set them

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
  // LB: the direction flipped, once a press
  if (shifted & BTN1_LB)
    reverse = !reverse;
  // X: the LEDs' next mode, once a press
  if (shifted & BTN1_X)
    ledMode = (ledMode + 1) % LED_MODES;

  // RB held brakes; else the throttle: RT released 0 (stopped) .. pulled
  // all the way DRIVE_MAX, times the gear's share, in LB's direction; the
  // same to the rear and the front wheels
  int16_t throttle = getReport(REPORT_RT) * DRIVE_MAX / 255;
  throttle = throttle * gears[gear] / 100;
  int8_t drive;
  if (buttons & BTN1_RB)
    drive = BRAKE;
  else if (reverse)
    drive = -throttle;
  else
    drive = throttle;
  setMotor(MOTOR_REAR, drive);
  setMotor(MOTOR_FRONT, drive);

  // the dumper: the right stick's Y (0 up, so negated: up lifts), up to
  // DRIVE_MAX either way; no gears
  setMotor(MOTOR_DUMPER,
           -centered(REPORT_RZ, DUMPER_DEADZONE) * DRIVE_MAX / 127);

  ledsShow(); // X's change at once; the blinking from tick()
}

// The pad's link lost (switched off, or out of range: nothing heard for the
// link's supervision timeout): the motors stopped, the wheels straight, and
// the LEDs blinking fast until the pad's next report runs loop() again (and
// they go back to the mode X set)
void onDisconnect() {
  setMotor(MOTOR_REAR, 0);
  setMotor(MOTOR_FRONT, 0);
  setMotor(MOTOR_DUMPER, 0);
  linkLost = true;
  ledsShow();
  setServo(SERVO_STEER, 150 - trim);
}
