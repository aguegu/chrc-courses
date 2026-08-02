Examples for Sourcekit MeshMass Series
===

Examples to customize firmware on Sourcekit&reg; MeshMass&trade; Series DevKits

`tx6a4d` is a RC tx/controller module that contains 6 analog and 4 digital channels.

`tx6ax` shares the same hardware with `tx6a4d`, but sends each analog input as a raw unsigned reading (0-255) instead of a signed value, leaving centering, deadzone and mixing to the receiver.

`rx4m4s` is a RC rx/vehicle module that could drive 4 DC motors and 4 servos.

`rx4m3s1n` share the same hardware with `rx4m4s`, but 1 servo channel is used to drive NeoPixel(WS2812) Leds.

`rx4m1s1n1a` share the same hardware with `rx4m4s` and `rx4m3s1n`, but 1 servo channel is used to drive NeoPixel(WS2812) Leds, another 2 servo channels are used to drive a Audio(MP3) module.

`rx4mx` is a single config-driven receiver that builds any `rx4m*` shape from two switches at the top of its `app.h` (Audio on SM1+SM2, NeoPixel on SM3). It consumes the raw unsigned protocol and pairs with `tx6ax`.

The firmware could be compiled and flashed to the DevKits on

  * https://meshmass.com (International)
  * https://meshmass.y77.cc (China Mainland)

This Repository is Published at

  * https://github.com/aguegu/chrc-courses
  * https://gitee.com/aGuegu/chrc-courses
