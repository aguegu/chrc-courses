#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

#define MOTOR_DRIVE 0 // throttle
#define SERVO_STEER 0 // steering (SM0)

// A single-stick RC car (throttle + steering folded onto the receiver):
//   right stick Y (ch1) -> throttle on motor 0, limited by the speed knob (ch4)
//   right stick X (ch0) -> steering on servo SM0, trimmed by the knob (ch5)
//   button 0 (ch6)      -> brake
void loop() {
  uint16_t speed = getChannel(4); // left knob: speed limit, 0..255

  // steering = main stick (ch0) + knob trim (ch5), mapped to a servo pulse.
  int16_t steer = -centered(0, 0) * 3 / 4 + centered(5, 0) / 4;
  setServo(SERVO_STEER, 150 + steer * 2 / 5);

  if (getChannel(6)) {
    setMotor(MOTOR_DRIVE, -128); // brake
  } else {
    setMotor(MOTOR_DRIVE, centered(1, 2) * speed / 255); // speed-limited throttle
  }
}
