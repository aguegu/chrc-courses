#include "app.h"

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

#define CHN_STEER       0 // right stick X: steering
#define CHN_DRIVE       2 // left stick Y: throttle
#define CHN_DRIVE_LIMIT 4 // left knob: speed limit
#define CHN_STEER_TRIM  5 // right knob: steering trim
#define CHN_BRAKE       6 // button 0: brake

#define MOTOR_DRIVE 0 // throttle
#define SERVO_STEER 0 // steering (SM0)

// A two-stick RC car (throttle and steering on separate sticks, mixed on the receiver):
//   left stick Y (ch2)  -> throttle on motor 0, limited by the speed knob (ch4)
//   right stick X (ch0) -> steering on servo SM0, trimmed by the knob (ch5)
//   button 0 (ch6)      -> brake
void loop() {
  uint16_t speed = getChannel(CHN_DRIVE_LIMIT); // left knob: speed limit, 0..255

  // steering = main stick + knob trim, mapped to a servo pulse.
  int16_t steer = -centered(CHN_STEER, 0) * 3 / 4 + centered(CHN_STEER_TRIM, 0) / 4;
  setServo(SERVO_STEER, 150 + steer * 2 / 5);

  if (getChannel(CHN_BRAKE)) {
    setMotor(MOTOR_DRIVE, -128); // brake
  } else {
    setMotor(MOTOR_DRIVE, centered(CHN_DRIVE, 2) * speed / 255); // speed-limited throttle
  }
}
