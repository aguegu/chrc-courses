RX4MX 音频 + 灯带 演示
===

本课把 **MP3 音频** 与 **NeoPixel 灯带** 同时打开：`app.h` 顶部两个开关都设为 `1`——SM1+SM2 接音频模块，SM3 接灯带。此时电机 4 路，舵机仅剩 SM0 一路。

这相当于 `10-NeoDemo`（灯带）与 `20-AudioDemo`（音频）的合体，两套 API 的详细说明见那两课：

  * 灯带：`neoSetup` / `neo()` / `neoSetHSL` / `neoSetColor` —— 见 `10-NeoDemo`。
  * 音频：`onPlayerReady` / `mpVolume` / `mpPlay` / `mpStop` —— 见 `20-AudioDemo`。

**本例效果**（见 `src/app.c`）：

  * `setup()` 里 `neoSetup(16)` 配置 16 颗 LED。
  * `onPlayerReady()` 把音量设为 `10`，并播放 `0001.mp3` 作为开机提示音。
  * `loop()`：4 路电机经 `centered()`（死区 8）驱动；SM0 舵机由旋钮（通道 4）控制。
  * `neo()`：一圈随 `step` 缓缓流动的彩虹，叠加一颗明亮的白色“游标”绕环移动。
