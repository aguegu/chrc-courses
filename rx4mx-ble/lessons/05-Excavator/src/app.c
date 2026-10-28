#include "app.h"

// Arm axes — the four motors.
#define MOTOR_BUCKET 0
#define MOTOR_BOOM   1
#define MOTOR_DIPPER 2
#define MOTOR_SWING  3

// Tracks + accessories — the four servos (each track feeds a brushed ESC).
#define SERVO_TRACK_R 0
#define SERVO_TRACK_L 1
#define SERVO_BLADE   2
#define SERVO_GRAB    3

// Center a raw 0..255 byte to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getReport(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

// The four shoulder buttons, as the report carries them: button 0 = LB,
// button 1 = View, button 2 = Menu, button 3 = RB
#define BUTTON0 (getReport(REPORT_BUTTONS1) & BTN1_LB)
#define BUTTON1 (getReport(REPORT_BUTTONS2) & BTN2_VIEW)
#define BUTTON2 (getReport(REPORT_BUTTONS2) & BTN2_MENU)
#define BUTTON3 (getReport(REPORT_BUTTONS1) & BTN1_RB)

// A forward/reverse button pair -> ESC servo pulse: 200 fwd, 100 rev, 150 stop.
static uint8_t track(bool fwd, bool rev) {
  if (fwd == rev) return 150; // both or neither
  return fwd ? 200 : 100;
}

void loop() {
  // Arm — right stick: bucket (X) / boom (Y); left stick: swing (X) / dipper (Y).
  // HID reads the sticks 0 to the left and up, the reverse of rx4mx's channels.
  setMotor(MOTOR_BUCKET, -centered(REPORT_Z, 0x20));
  setMotor(MOTOR_BOOM, centered(REPORT_RZ, 0x20));
  setMotor(MOTOR_DIPPER, centered(REPORT_Y, 0x20));
  setMotor(MOTOR_SWING, -centered(REPORT_X, 0x20) / 2);

  // Tracks — shoulder buttons via ESC servos: left = button 1/0 (fwd/rev),
  // right = button 2/3 (fwd/rev).
  setServo(SERVO_TRACK_L, track(BUTTON1, BUTTON0));
  setServo(SERVO_TRACK_R, track(BUTTON2, BUTTON3));

  // Blade (left knob, LT) and grabber (right knob, RT) -> servo pulse 100..200.
  setServo(SERVO_BLADE, 100 + getReport(REPORT_LT) * 100 / 255);
  setServo(SERVO_GRAB, 100 + getReport(REPORT_RT) * 100 / 255);
}
