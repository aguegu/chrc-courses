RX4MX NeoPixel 演示
===

本课在 `rx4mx` 上点亮 **NeoPixel（WS2812）** 灯带。把 `app.h` 顶部的开关设为 `NEO_ON_SM3 1`，SM3 排针即从“舵机”切换为“灯带”输出；此时电机仍为 4 路，舵机变为 3 路（SM0/SM1/SM2）。

**灯带相关函数**

`neoSetup(pixelCount)`：在 `setup()` 中调用一次，设置灯带上 LED 的数量（1–32）。本例为 16。

`neo()`：灯带动画回调，由系统以约 `8Hz`（每 125ms 一次）的频率自动调用，与主控制循环 `loop()` 相互独立——即使控制数据更新得慢，动画也能流畅播放。在这里编写逐帧的灯效。

设置某个 LED 的颜色有两种方式：

`neoSetHSL(n, hue, saturation, lightness)` —— 用 HSL（色相 / 饱和度 / 亮度）模型：

  * `n`：LED 序号（`0` 到 `pixelCount - 1`）
  * `hue`：色相，`0–359` 度
  * `saturation`：饱和度，`0`（灰）–`255`（最鲜艳）
  * `lightness`：亮度，`0`（灭）–`255`（最亮）

`neoSetColor(n, color, lightness)` —— 简化版：`color` 取 `0–359` 为色相（满饱和），取 `360` 为白色；`lightness` 含义同上。把 `lightness` 设为 `0` 即可熄灭该 LED。

> 也可直接使用预定义的色相常量：`COLOR_RED / COLOR_ORANGE / COLOR_YELLOW / COLOR_GREEN / COLOR_TEAL / COLOR_BLUE / COLOR_INDIGO / COLOR_PURPLE / COLOR_PINK / COLOR_WHITE`。

**本例效果**（见 `src/app.c`）：一个点在灯环上循环移动，移动的同时色相不断变化，其余 LED 熄灭。控制部分：4 路摇杆经 `centered()` 驱动电机；两个旋钮（通道 4、5）驱动舵机 SM0/SM1；按键 0（通道 6）驱动舵机 SM2 —— 松开为 `100`（1.0ms），按下为 `200`（2.0ms），演示接收端如何读取“按键”通道（在 `tx6ax` 上按键位于通道 6–9）。

> 提示：亮度 `lightness` 不宜过高（本例用 `0x08`）；灯带越长、越亮，功耗越大。
