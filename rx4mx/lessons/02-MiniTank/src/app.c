#include "app.h"

#ifndef MAX
#define MAX(n, m) (((n) < (m)) ? (m) : (n))
#endif

// Center a raw 0..255 stick to a signed -127..+127 swing (see 00-Default).
static int8_t centered(uint8_t index, uint8_t deadzone) {
  int16_t r = getChannel(index);
  int16_t s = r <= 127 ? r - 127 : r - 128;
  if (s > deadzone) return (s - deadzone) * 127 / (127 - deadzone);
  if (s < -deadzone) return (s + deadzone) * 127 / (127 - deadzone);
  return 0;
}

static int16_t maxIn3(int16_t a, int16_t b, int16_t c) {
  int16_t m = MAX(a, b);
  return MAX(m, c);
}

#define STK_Y_R 1 // right stick, vertical
#define STK_X_R 0 // right stick, horizontal
#define STK_Y_L 2 // left stick, vertical
#define STK_X_L 3 // left stick, horizontal
#define STK_L 2   // tank mode: left track
#define STK_R 1   // tank mode: right track
#define STK_MODE 5  // right knob: pick the drive mode
#define STK_SPEED 4 // left knob: overall speed limit

// A tank/vehicle driver with three mixing modes, chosen by the mode knob:
//   knob left  -> arcade on the LEFT stick   (Y = throttle, X = turn)
//   knob right -> arcade on the RIGHT stick
//   knob centered -> tank: each stick drives one track directly
// Left track -> motor 0, right track -> motor 1 (negated for mirrored wiring).
void loop() {
  uint16_t speed = getChannel(STK_SPEED) / 2; // 0..127
  int8_t mode = centered(STK_MODE, 0);

  if (mode < -0x40) { // arcade, right stick
    int16_t left = centered(STK_Y_R, 5) - centered(STK_X_R, 5);
    int16_t right = centered(STK_Y_R, 5) + centered(STK_X_R, 5);
    uint16_t m = maxIn3(abs(left), abs(right), 127);
    setMotor(0, +left * speed / m);
    setMotor(1, -right * speed / m);
  } else if (mode > 0x40) { // arcade, left stick
    int16_t left = centered(STK_Y_L, 5) - centered(STK_X_L, 5);
    int16_t right = centered(STK_Y_L, 5) + centered(STK_X_L, 5);
    uint16_t m = maxIn3(abs(left), abs(right), 127);
    setMotor(0, +left * speed / m);
    setMotor(1, -right * speed / m);
  } else { // tank
    setMotor(0, +centered(STK_L, 5) * speed / 254);
    setMotor(1, -centered(STK_R, 5) * speed / 254);
  }
}
