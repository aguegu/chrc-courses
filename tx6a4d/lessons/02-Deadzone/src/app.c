#include "app.h"

// 遥控器：
// 按键： Buttons[3], Buttons[2], Buttons[1], Buttons[0] (从左到右)
// 左摇杆：                右摇杆：
//    上下： Sticks[2]        上下： Sticks[1]
//    左右： Sticks[3]        左右： Sticks[0]
// 左旋钮： Sticks[4]      右旋钮： Sticks[5]

// 接收机：
// 舵机： Channels[4], Channels[5], Channels[6], Channels[7] (从左到右)
// 电机： Channels[0], Channels[1], Channels[2], Channels[3] (从左到右)

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#define DEADZONE (5)

int8_t getStickNeutral(uint8_t index) {
  return ABS(getStick(index)) > DEADZONE ? getStick(index) : 0;
}

void app() {
  setChannel(0, getStickNeutral(2));
  setChannel(1, getStickNeutral(3));
  setChannel(2, getStickNeutral(0));
  setChannel(3, getStickNeutral(1));

  setChannel(4, getStick(4));
  setChannel(5, getStick(5));

  if (getButton(2) == getButton(3)) {
    setChannel(6, 0);
  } else if (getButton(2)) {
    setChannel(6, 127);
  } else {  // getButton(3)
    setChannel(6, -127);
  }

  if (getButton(1) == getButton(0)) {
    setChannel(7, 0);
  } else if (getButton(1)) {
    setChannel(7, 127);
  } else {  // getButton(0)
    setChannel(7, -127);
  }
}
