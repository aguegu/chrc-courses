#include "app.h"

// Motors this forklift drives.
#define MOTOR_DRIVE 0 // fwd / back
#define MOTOR_LIFT  1 // fork up / down
#define MOTOR_PITCH 2 // fork tilt (pitch)
#define SERVO_STEER 0 // steering (SM0)

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

// Squared stick response — gentle near center, full power at the ends.
static int8_t stickToMotor(uint8_t index, uint8_t deadzone) {
  float f = centered(index, deadzone) / 127.0f;
  return (int8_t)(f * f * 127.0f * (f > 0 ? 1 : -1));
}

void loop() {
  uint16_t throttle = getChannel(4); // left knob: speed limit 0..255

  // --- drive + steering: the 03-Combo base ---
  if (getChannel(7)) {               // engine brake (button 1)
    setMotor(MOTOR_DRIVE, -128);
  } else {                           // fwd/back on left stick Y, speed-limited
    setMotor(MOTOR_DRIVE, -stickToMotor(2, 5) * throttle / 255);
  }

  // steering: right stick X + bias knob (ch5) -> servo SM0
  int16_t steer = centered(0, 5) * 2 / 3 + centered(5, 0) / 3;
  setServo(SERVO_STEER, 150 + steer * 2 / 5);

  // --- forklift arm ---
  setMotor(MOTOR_PITCH, stickToMotor(3, 0x20) / 4); // pitch on left stick X

  if (getChannel(8)) {               // lift brake (button 2)
    setMotor(MOTOR_LIFT, -128);
  } else {                           // lift on right stick Y
    setMotor(MOTOR_LIFT, stickToMotor(1, 5) / 4);
  }
}
