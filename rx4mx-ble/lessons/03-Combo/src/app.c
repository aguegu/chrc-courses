#include "app.h"

// Center a raw 0..255 byte to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getReport(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

#define CHN_STEER       REPORT_Z  // right stick X: steering (0 left)
#define CHN_DRIVE       REPORT_Y  // left stick Y: throttle (0 up)
#define CHN_DRIVE_LIMIT REPORT_LT // left knob: speed limit
#define CHN_STEER_TRIM  REPORT_RT // right knob: steering trim

#define MOTOR_DRIVE 0 // throttle
#define SERVO_STEER 0 // steering (SM0)

// L (button 0, LB in the report): brake
static bool brake() { return getReport(REPORT_BUTTONS1) & BTN1_LB; }

// A two-stick RC car (throttle and steering on separate sticks, mixed on the receiver):
//   left stick Y   -> throttle on motor 0, limited by the speed knob (LT)
//   right stick X  -> steering on servo SM0, trimmed by the knob (RT)
//   L (button 0)   -> brake
void loop() {
  uint16_t speed = getReport(CHN_DRIVE_LIMIT); // left knob: speed limit, 0..255

  // steering = main stick + knob trim, mapped to a servo pulse.
  int16_t steer = centered(CHN_STEER, 0) * 3 / 4 + centered(CHN_STEER_TRIM, 0) / 4;
  setServo(SERVO_STEER, 150 + steer * 2 / 5);

  if (brake()) {
    setMotor(MOTOR_DRIVE, -128); // brake
  } else {
    setMotor(MOTOR_DRIVE, -centered(CHN_DRIVE, 2) * speed / 255); // speed-limited throttle, up = forward
  }
}
