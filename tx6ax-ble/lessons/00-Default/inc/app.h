#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Read digital button state.
 *
 * @param index Button index (0-3):
 * • 0: Button 0 (left edge) / Left joystick press
 * • 1: Button 1 (left middle)
 * • 2: Button 2 (right middle)
 * • 3: Button 3 (right edge) / Right joystick press
 * @return true if button is pressed, false otherwise
 */
bool getButton(uint8_t index);

/**
 * Read analog input value.
 *
 * @param index Analog input index (0-5):
 * • 0: Right joystick X-axis (STK0)
 * • 1: Right joystick Y-axis (STK1)
 * • 2: Left joystick Y-axis (STK2)
 * • 3: Left joystick X-axis (STK3)
 * • 4: Left knob (STK4)
 * • 5: Right knob (STK5)
 * @return Raw unsigned 8-bit reading (0 to 255) from the input's own ADC
 * channel, VBat-compensated for battery droop. ~128 is the center/neutral
 * position; 0 and 255 are the travel extremes.
 */
uint8_t getStick(uint8_t index);

// The gamepad report, a ShanWan pad's: the bytes...
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
 * Set a byte of the gamepad report sent to the host, as it goes (the
 * REPORT_ bytes above; rx4mx-ble's getReport reads it back the same).
 *
 * @param index Byte index (0 to REPORT_LEN - 1; beyond it ignored)
 * @param value The byte: a stick or trigger 0-255; the hat in its low 4 bits
 * (HAT_REST at rest); the buttons as bits (e.g. BTN1_A | BTN1_LB).
 */
void setReport(uint8_t index, uint8_t value);

/**
 * Get a byte of the gamepad report as loop() last set it.
 *
 * @param index Byte index (0 to REPORT_LEN - 1; beyond it reads 0)
 * @return The byte. Until loop() sets them, the sticks read centred (0x80),
 * the hat at rest, the rest 0.
 */
uint8_t getReport(uint8_t index);

/**
 * Mute or unmute the buzzer. The firmware sounds it while the host's link is
 * lost and quiets it when a host links again; mute it if that alarm annoys
 * you (call it in setup() to start muted). Unmuted again while the link is
 * lost, it sounds.
 *
 * @param enable false to mute, true to unmute (the default).
 */
void enableBuzzer(bool enable);

/**
 * Initialization function called once at system startup.
 *
 * Implement this function to perform one-time setup tasks such as
 * initializing variables or muting the buzzer (enableBuzzer(false)).
 */
void setup();

/**
 * Application main loop function.
 *
 * Called every 20ms (50Hz) by the firmware scaffold. Implement this function
 * to define your control logic mapping inputs to the gamepad report.
 *
 * Important: Avoid busy-waiting or delay functions. The firmware runs a
 * Real-Time Operating System (RTOS) in the background.
 *
 * After loop() completes, the firmware sends the gamepad report to the
 * linked host, as it is. Bytes not set in loop() keep their previous values.
 */
void loop();

/**
 * Called when the host's link is lost (switched off, out of range: no
 * packet for the link's supervision timeout). The firmware sounds the
 * buzzer itself (enableBuzzer() mutes it); implement this for anything else
 * the app should do then. Optional.
 */
void onDisconnect();

#endif
