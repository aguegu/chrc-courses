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

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

// A forward/reverse button pair -> ESC servo pulse: 200 fwd, 100 rev, 150 stop.
static uint8_t track(uint8_t fwdCh, uint8_t revCh) {
  if (getChannel(fwdCh) == getChannel(revCh)) return 150; // both or neither
  return getChannel(fwdCh) ? 200 : 100;
}

void loop() {
  // Arm — right stick: bucket (X) / boom (Y); left stick: swing (X) / dipper (Y).
  setMotor(MOTOR_BUCKET, centered(0, 0x20));
  setMotor(MOTOR_BOOM, -centered(1, 0x20));
  setMotor(MOTOR_DIPPER, -centered(2, 0x20));
  setMotor(MOTOR_SWING, centered(3, 0x20) / 2);

  // Tracks — shoulder buttons via ESC servos: left = btn1/btn0 (ch7/ch6 fwd/rev),
  // right = btn2/btn3 (ch8/ch9 fwd/rev).
  setServo(SERVO_TRACK_L, track(7, 6));
  setServo(SERVO_TRACK_R, track(8, 9));

  // Blade (left knob ch4) and grabber (right knob ch5) -> servo pulse 100..200.
  setServo(SERVO_BLADE, 100 + getChannel(4) * 100 / 255);
  setServo(SERVO_GRAB, 100 + getChannel(5) * 100 / 255);
}
