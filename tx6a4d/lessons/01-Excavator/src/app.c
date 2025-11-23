#include "app.h"

// 遥控器：
//
// 右摇杆：
//    左右： Sticks[0]
//    上下： Sticks[1]
// 左摇杆：
//    上下： Sticks[2]
//    左右： Sticks[3]
// 左旋钮： Sticks[4]
// 右旋钮： Sticks[5]
// 按键从左到右： Buttons[3], Buttons[2], Buttons[1], Buttons[0]

// 接收机：
//
// 电机从左到右： Channels[0], Channels[1], Channels[2], Channels[3]
// 舵机从左到右： Channels[4], Channels[5], Channels[6], Channels[7]

void app() {
  setChannel(0, getStick(2));
  setChannel(1, getStick(3));
  setChannel(2, getStick(0));
  setChannel(3, getStick(1));

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
