#include "app.h"

// The tx6a4d as a gamepad: its inputs into the gamepad report, a ShanWan
// pad's (app.h's REPORT_ and BTN names):
//   the sticks into X, Y (left) and Z, Rz (right), flipped to HID's
//   directions
//   the knobs into the triggers: the left (STK4, a model's maximum speed,
//   typically) as LT, the right (STK5, its trim, typically) as RT
//   the four shoulder buttons into LB, View, Menu, RB
//   each knob's quarter as a mode 0-3 (LT >> 6, RT >> 6), its high bit and
//   low bit into buttons 9 and 13 (LT), 10 and 16 (RT)
//   the hat at rest (the tx6a4d has none)

#define STK_X_R 0 // the tx6a4d's sticks and knobs, by getStick's index
#define STK_Y_R 1
#define STK_Y_L 2
#define STK_X_L 3
#define KNOB_L 4
#define KNOB_R 5

#define BTN_L 0  // the shoulder buttons, by getButton's index: left edge
#define BTN_L2 1 // left middle
#define BTN_R2 2 // right middle
#define BTN_R 3  // right edge

// the modes' bits, in REPORT_BUTTONS2: buttons 9 and 10 (the Q34P's LT and
// RT pressed), 13 and 16 (none on the Q34P)
#define LT_MODE_HIGH BTN2_LT // button 9
#define LT_MODE_LOW 0x10     // button 13
#define RT_MODE_HIGH BTN2_RT // button 10
#define RT_MODE_LOW 0x80     // button 16

// an axis as HID has it: the tx6a4d's sticks read high pushed up and to the
// left, where HID wants 0
static uint8_t flip(uint8_t v) { return 255 - v; }

void loop() {
  setReport(REPORT_X, flip(getStick(STK_X_L)));
  setReport(REPORT_Y, flip(getStick(STK_Y_L)));
  setReport(REPORT_Z, flip(getStick(STK_X_R)));
  setReport(REPORT_RZ, flip(getStick(STK_Y_R)));
  setReport(REPORT_HAT, HAT_REST);
  setReport(REPORT_BUTTONS1, (getButton(BTN_L) ? BTN1_LB : 0) |
                                 (getButton(BTN_R) ? BTN1_RB : 0));
  uint8_t ltMode = getStick(KNOB_L) >> 6, rtMode = getStick(KNOB_R) >> 6;
  setReport(REPORT_BUTTONS2, (getButton(BTN_L2) ? BTN2_VIEW : 0) |
                                 (getButton(BTN_R2) ? BTN2_MENU : 0) |
                                 (ltMode >> 1 ? LT_MODE_HIGH : 0) |
                                 (ltMode & 1 ? LT_MODE_LOW : 0) |
                                 (rtMode >> 1 ? RT_MODE_HIGH : 0) |
                                 (rtMode & 1 ? RT_MODE_LOW : 0));
  setReport(REPORT_RT, getStick(KNOB_R));
  setReport(REPORT_LT, getStick(KNOB_L));
}

// The link lost: the buzzer alerts the user, until a host links again
void onDisconnect() { setBuzzer(true); }
