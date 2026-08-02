#include "app.h"

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

// Button-controlled state (toggles), plus lighting state from the driving.
static bool headOn = false; // btn0 toggles the headlights
static bool soundOn = true; // btn2 toggles the sound
static bool flashOn, brakeOn, reverseOn, leftOn, rightOn;

void setup() {
  neoSetup(12);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {
  static bool lightLast, soundLast;

  // buttons (tx6ax maps btn0..3 -> channels 6..9)
  bool lightBtn = getChannel(6); // btn0: headlights on/off (toggle)
  flashOn = getChannel(7);       // btn1: headlight flash (hold)
  bool soundBtn = getChannel(8); // btn2: sound on/off (toggle)
  bool brakeBtn = getChannel(9); // btn3: brake (hold)

  if (lightBtn && !lightLast) headOn = !headOn; // toggle on the press edge
  lightLast = lightBtn;

  if (soundBtn && !soundLast) {
    soundOn = !soundOn;
    if (!soundOn) mpStop();
  }
  soundLast = soundBtn;

  // drive: fwd/back on left stick Y, speed-limited; brake button forces a stop
  uint16_t throttle = getChannel(4);
  int8_t drive = brakeBtn ? -128 : -stickToMotor(2, 5) * throttle / 255;
  setMotor(0, drive);

  // steering: right stick X + bias knob (ch5) -> servo SM0
  int8_t steer = centered(0, 5) * 2 / 3 + centered(5, 0) / 3;
  setServo(0, 150 + steer * 2 / 5);

  // dump bed: raise / lower on right stick Y
  int8_t bed = stickToMotor(1, 5) / 2;
  setMotor(1, bed);

  // lighting state
  reverseOn = drive > 0;
  brakeOn = brakeBtn || reverseOn;
  leftOn = steer > 20;
  rightOn = steer < -20;

  // sound: only while enabled; event cues interrupt, engine loop fills gaps.
  if (soundOn) {
    if (leftOn || rightOn) mpPlay(3, true); // 0003.mp3 turn signal
    if (bed != 0)          mpPlay(2, true); // 0002.mp3 dumping / hydraulics
    mpPlay(5, false);                       // 0005.mp3 engine idle loop
  }
}

// 12-pixel truck lighting, in the 04-Crawler style: a dim running-light base
// while the headlights are on, bright overlays on events, blinking turn
// signals. Physical layout:
//   headlights 0,1,4,5   left turn 2,9   right turn 3,8
//   reverse 6,7          brake 10,11
void neo() {
  static uint8_t step = 0;
  bool blink = step & 0x02;
  uint8_t base = headOn ? 0x08 : 0x00; // running-light glow

  // headlights: steady when on; btn1 flashes them bright (flash-to-pass)
  uint8_t head = flashOn ? (blink ? 0xC0 : 0) : (headOn ? 0x20 : 0);
  neoSetColor(0, COLOR_WHITE, head);
  neoSetColor(1, COLOR_WHITE, head);
  neoSetColor(4, COLOR_WHITE, head);
  neoSetColor(5, COLOR_WHITE, head);

  uint8_t lt = base + (leftOn && blink ? 0x60 : 0);
  neoSetColor(2, COLOR_ORANGE, lt);
  neoSetColor(9, COLOR_ORANGE, lt);

  uint8_t rt = base + (rightOn && blink ? 0x60 : 0);
  neoSetColor(3, COLOR_ORANGE, rt);
  neoSetColor(8, COLOR_ORANGE, rt);

  uint8_t rev = base + (reverseOn ? 0x40 : 0);
  neoSetColor(6, COLOR_WHITE, rev);
  neoSetColor(7, COLOR_WHITE, rev);

  uint8_t brk = base + (brakeOn ? 0x80 : 0);
  neoSetColor(10, COLOR_RED, brk);
  neoSetColor(11, COLOR_RED, brk);

  step++;
}
