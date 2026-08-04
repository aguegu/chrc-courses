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
static bool soundOn = false;  // btn2 toggles the sound (muted at start)
static bool hazardOn = false; // btn1 toggles the hazard (warning) lights
static bool brakeOn, reverseOn, leftOn, rightOn;

void setup() {
  neoSetup(12);
}

void onPlayerReady() {
  mpVolume(15);
}

void loop() {
  static bool lightLast, hazardLast, soundLast;

  // buttons (tx6ax maps btn0..3 -> channels 6..9)
  bool lightBtn = getChannel(6);  // btn0: headlights on/off (toggle)
  bool hazardBtn = getChannel(7); // btn1: hazard lights on/off (toggle)
  bool soundBtn = getChannel(8);  // btn2: sound on/off (toggle)
  bool brakeBtn = getChannel(9);  // btn3: brake (hold)

  if (lightBtn && !lightLast) headOn = !headOn; // toggle on the press edge
  lightLast = lightBtn;

  if (hazardBtn && !hazardLast) hazardOn = !hazardOn;
  hazardLast = hazardBtn;

  if (soundBtn && !soundLast) {
    soundOn = !soundOn;
    if (soundOn) mpPlay(1, true); // engine start; 0005 loop takes over after
    else mpPlay(4, true);         // engine stop (one shot, then silence)
  }
  soundLast = soundBtn;

  // drive: fwd/back on right stick Y, speed-limited; brake button forces a stop
  uint16_t throttle = getChannel(4);
  int8_t drive = brakeBtn ? -128 : -stickToMotor(1, 5) * throttle / 255;
  setMotor(0, drive);

  // steering: right stick X + bias knob (ch5) -> servo SM0
  int8_t steer = centered(0, 5) * 2 / 3 + centered(5, 0) / 3;
  setServo(0, 150 + steer * 2 / 5);

  // dump bed: raise / lower on left stick X
  int8_t bed = stickToMotor(3, 5) / 2;
  setMotor(1, bed);

  // lighting state
  reverseOn = drive > 0;
  brakeOn = brakeBtn || reverseOn;
  leftOn = steer > 0x40;
  rightOn = steer < -0x40;

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
//   reverse 6,11         brake 7,10
void neo() {
  static uint8_t step = 0;
  bool blink = step & 0x02; // turn-signal cadence
  uint8_t base = headOn ? 0x08 : 0x00; // running-light glow

  // headlights: steady full-brightness white when on
  uint8_t head = headOn ? 0xFF : 0;
  neoSetColor(0, COLOR_WHITE, head);
  neoSetColor(1, COLOR_WHITE, head);
  neoSetColor(4, COLOR_WHITE, head);
  neoSetColor(5, COLOR_WHITE, head);

  // turn signals; the hazard toggle blinks both sides together
  uint8_t lt = base + ((leftOn || hazardOn) && blink ? 0x60 : 0);
  neoSetColor(2, COLOR_ORANGE, lt);
  neoSetColor(9, COLOR_ORANGE, lt);

  uint8_t rt = base + ((rightOn || hazardOn) && blink ? 0x60 : 0);
  neoSetColor(3, COLOR_ORANGE, rt);
  neoSetColor(8, COLOR_ORANGE, rt);

  // reverse cluster: red tail with the running lights, white while reversing
  uint8_t rev = base + (reverseOn ? 0x40 : 0);
  uint16_t revColor = reverseOn ? COLOR_WHITE : COLOR_RED;
  neoSetColor(6, revColor, rev);
  neoSetColor(11, revColor, rev);

  uint8_t brk = base + (brakeOn ? 0x80 : 0);
  neoSetColor(7, COLOR_RED, brk);
  neoSetColor(10, COLOR_RED, brk);

  step++;
}
