#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// ===========================================================================
// Layer 1 — Board survey
// ---------------------------------------------------------------------------
// Answer two questions about your board; everything else is derived below.
//
// The PCB has four servo headers: SM0 SM1 SM2 SM3. Optional modules claim
// headers — the MP3 module uses SM1 + SM2, the neopixel uses SM3. Whatever
// headers are left over become your servo channels.
// ===========================================================================

#define AUDIO_ON_SM1_SM2 0 // 1 = MP3 module on SM1+SM2; 0 = SM1, SM2 are servos
#define NEO_ON_SM3 0       // 1 = neopixel on SM3;       0 = SM3 is a servo

// ===========================================================================
// Layer 2 — Derived configuration (generated from the survey; don't edit)
// ---------------------------------------------------------------------------
//   audio neo  servos
//    no    no  SM0 SM1 SM2 SM3 (4)
//    no   yes  SM0 SM1 SM2     (3)
//   yes   yes  SM0             (1)
//   yes    no  SM0 SM3         (2*)   (*sparse: index 0 and 3)
// ===========================================================================

// Four headers minus those claimed by fitted modules (audio takes 2, neo 1).
#define SERVO_COUNT (4 - AUDIO_ON_SM1_SM2 * 2 - NEO_ON_SM3)

// Which SM headers are servos: each entry holds its own SM label, or 0xFF if
// the header is claimed by a module. main.c compacts this to a servoTargets
// slot (the header's rank among the servos).
#define SM_SLOTS                                                               \
  {0, AUDIO_ON_SM1_SM2 ? 0xFF : 1, AUDIO_ON_SM1_SM2 ? 0xFF : 2,                \
   NEO_ON_SM3 ? 0xFF : 3}

// ===========================================================================
// Public API
// ===========================================================================

// The gamepad report, in Bluetooth mode (meshmass/ble-report.md, a row
// each): the bytes...
#define REPORT_LEN 10
#define REPORT_X 0        // left stick, horizontal: 0 left, 255 right
#define REPORT_Y 1        // left stick, vertical: 0 up, 255 down
#define REPORT_Z 2        // right stick, horizontal: 0 left, 255 right
#define REPORT_RZ 3       // right stick, vertical: 0 up, 255 down
#define REPORT_HAT 4      // the D-pad, low 4 bits (HAT_MASK)
#define REPORT_BUTTONS1 5 // buttons 1-8 (the BTN1_ masks)
#define REPORT_BUTTONS2 6 // buttons 9-16 (the BTN2_ masks)
#define REPORT_RT 7 // RT, analog (HID's Accelerator): 0 released, 255 pulled
#define REPORT_LT 8 // LT, analog (HID's Brake): 0 released, 255 pulled
// ...the hat: 0 up, 1 up-right, 2 right .. 7 up-left, above 7 at rest
#define HAT_MASK 0x0F
#define HAT_REST 0x0F
// ...and the buttons, by the ShanWan Q34P's names: in REPORT_BUTTONS1
#define BTN1_A 0x01
#define BTN1_B 0x02
#define BTN1_X 0x08
#define BTN1_Y 0x10
#define BTN1_LB 0x40
#define BTN1_RB 0x80
// in REPORT_BUTTONS2
#define BTN2_LT 0x01 // LT pressed (with REPORT_LT's analog value)
#define BTN2_RT 0x02 // RT pressed (with REPORT_RT's)
#define BTN2_VIEW 0x04
#define BTN2_MENU 0x08
#define BTN2_L3 0x20 // the left stick pressed
#define BTN2_R3 0x40 // the right stick pressed

/**
 * Read a byte of the gamepad's latest report, as it came (the ShanWan
 * layout: the REPORT_ bytes above; meshmass/ble-report.md has the table).
 *
 * @param index Byte index (0 to REPORT_LEN - 1; beyond it reads 0)
 * @return The byte: a stick or trigger 0-255 (sticks ~128 at center); the
 * hat in its low 4 bits; the buttons as bits (e.g. getReport(REPORT_BUTTONS1)
 * & BTN1_A). Until the first report, the sticks centred and the rest 0. The
 * receiver owns any centering, deadzone, scaling, or mixing.
 */
uint8_t getReport(uint8_t index);

/**
 * Set motor output value.
 *
 * @param index Motor index (0-3)
 * @param value Signed 8-bit value (-128 to 127) representing motor speed and direction.
 * The value represents PWM duty cycle percentage:
 * • 127: 100% PWM duty cycle in one direction
 * • 0: stop (no power)
 * • -127: 100% PWM duty cycle in opposite direction
 * • -128: activates braking (short circuit)
 * Actual rotation direction depends on motor wiring.
 * Swap motor terminals to reverse direction if needed.
 */
void setMotor(uint8_t index, int8_t value);

/**
 * Set servo output value.
 *
 * @param index Servo index — the PCB SM header label. Available headers
 * depend on the survey (e.g. 0 and 3 when audio is fitted). Indices that
 * aren't servo headers on this board are ignored.
 * @param value Unsigned 8-bit value (0-255) representing servo position.
 * The value corresponds to PWM high-level pulse width in
 * 0.01ms units within a 20ms period, providing a range of
 * 0ms to 2.55ms with 0.01ms resolution.
 * Typical analog servos use pulse widths between 1.0ms and 2.0ms:
 * • 100 = 1.0ms pulse
 * • 150 = 1.5ms pulse (center position for regular analog servos)
 * • 200 = 2.0ms pulse
 * Advanced servos may support extended ranges (0.5ms to 2.5ms).
 * The mapping between pulse width and physical angle varies
 * by servo model and should be calibrated for each application.
 */
void setServo(uint8_t index, uint8_t value);

/**
 * Get current motor output value.
 *
 * @param index Motor index (0-3)
 * @return Signed 8-bit value (-128 to 127) representing the current
 * motor output value set by setMotor().
 */
int8_t getMotor(uint8_t index);

/**
 * Get current servo output value.
 *
 * @param index Servo index — the PCB SM header label (see setServo()).
 * @return Unsigned 8-bit value (0-255) representing the current
 * servo output value set by setServo().
 */
uint8_t getServo(uint8_t index);

/**
 * Initialization function called once at system startup.
 *
 * Implement this function to perform one-time setup tasks such as
 * initializing variables, configuring hardware peripherals, or
 * setting default motor/servo positions.
 */
void setup();

/**
 * Main application loop function called each time a report arrives from the
 * paired gamepad (a pad sends one when anything on it changes).
 *
 * Implement this function to map the received report (via getReport())
 * to motor and servo outputs (using setMotor() and setServo()).
 *
 * After loop() completes, the firmware updates all motor and servo outputs
 * based on the values set during this execution.
 */
void loop();

/**
 * Called when the gamepad's link is lost: switched off, or out of range (no
 * packet for the link's supervision timeout, 1 s). Implement to put
 * actuators into a safe state (e.g. zero motors, center servos). The pad's
 * next report runs loop() again.
 */
void onDisconnect();

#if AUDIO_ON_SM1_SM2
/**
 * Play an audio file on the attached MP3 module.
 *
 * @param filesn File number (1-65535) as stored on the module's SD/flash.
 * @param force  If true, re-triggers the same file even while it's
 * already playing the same file — useful when you want
 * the effect to restart on every input edge. If false,
 * only starts playback when the module is idle (BUSY
 * line low). The call is rate-limited internally to
 * once per 80 ms regardless of `force`.
 */
void mpPlay(uint16_t filesn, bool force);

/**
 * Stop playback on the attached MP3 module.
 *
 * Halts the current track immediately (BUSY line returns low). After
 * a stop, the next mpPlay() — with or without `force` — starts cleanly
 * from idle.
 */
void mpStop(void);

/**
 * Set the audio module's playback volume.
 *
 * @param value Volume level 0–30 (module has 31 steps including mute).
 * Values ≥ 31 wrap modulo 31.
 */
void mpVolume(uint8_t value);

/**
 * Set the audio module's loop mode.
 *
 * @param isLoop true = loop current file; false = single-play then stop.
 */
void mpLoop(bool isLoop);

/**
 * Called once the audio module has finished its power-on sequence
 * (~1 s after boot). Typical use is to send the initial volume and
 * play a boot jingle. Default is a weak stub — override to customize.
 */
void onPlayerReady();
#endif // AUDIO_ON_SM1_SM2

#if NEO_ON_SM3
/*
 * Common color hue values for use with neoSetHSL() and neoSetColor() functions.
 * Hue values range from 0-359 degrees on the color wheel.
 */
#define COLOR_RED 0
#define COLOR_ORANGE 25
#define COLOR_YELLOW 55
#define COLOR_GREEN 120
#define COLOR_TEAL 170
#define COLOR_BLUE 210
#define COLOR_INDIGO 240
#define COLOR_PURPLE 275
#define COLOR_PINK 330
#define COLOR_WHITE 360

/**
 * Neopixel animation function called regularly by the operating system,
 * every 125ms (8 times per second).
 *
 * Implement this function to create dynamic LED animations, color patterns,
 * or visual feedback. This function is called independently of the main
 * control loop, allowing smooth animations even when control updates are
 * infrequent.
 */
void neo();

/**
 * Initialize Neopixel LED strip configuration.
 *
 * Call this function within setup() to configure the number of LEDs
 * in the connected Neopixel strip. Must be called before using any
 * other Neopixel functions.
 *
 * @param pixelCount Number of LEDs in the Neopixel strip (1-32).
 */
void neoSetup(uint8_t pixelCount);

/**
 * Set Neopixel LED color using HSL (Hue, Saturation, Lightness) color model.
 *
 * @param n LED index (0 to pixelCount-1, as configured by neoSetup())
 * @param hue Hue value (0-359 degrees on color wheel)
 * @param saturation Saturation value (0-255, 0 = grayscale, 255 = full color)
 * @param lightness Lightness value (0-255, 0 = off, 255 = maximum brightness)
 */
void neoSetHSL(uint8_t n, uint16_t hue, uint8_t saturation, uint8_t lightness);

/**
 * Set Neopixel LED color using simplified color value.
 *
 * This is a convenience function that calls neoSetHSL() with simplified
 * parameters. When color is in range 0-359, it's equivalent to
 * neoSetHSL(index, color, 255, lightness) (full saturation). When color
 * equals 360, it's equivalent to neoSetHSL(index, 0, 0, lightness) (white,
 * saturation zero).
 *
 * @param index LED index (0 to pixelCount-1, as configured by neoSetup())
 * @param color Color value (0-359 for hue, 360 for white)
 * @param lightness Lightness value (0-255, 0 = off, 255 = maximum brightness)
 */
void neoSetColor(uint8_t index, uint16_t color, uint8_t lightness);
#endif // NEO_ON_SM3

#endif
