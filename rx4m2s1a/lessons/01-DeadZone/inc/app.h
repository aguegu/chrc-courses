#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Read current wireless channel value received from transmitter.
 *
 * @param index Channel index (0-15)
 * @return Signed 8-bit value (-128 to 127) representing the current
 * channel value received from the paired transmitter.
 */
int8_t getChannel(uint8_t index);

/**
 * Set motor output value.
 *
 * @param index Motor index (0-3 for RX4M2S1A)
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
 * @param index Servo index — PCB header label: 0 (SM0) or 3 (SM3).
 * SM1/SM2 carry the audio module; other indices are
 * ignored.
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
 * @param index Motor index (0-3 for RX4M2S1A)
 * @return Signed 8-bit value (-128 to 127) representing the current
 * motor output value set by setMotor().
 */
int8_t getMotor(uint8_t index);

/**
 * Get current servo output value.
 *
 * @param index Servo index — PCB header label: 0 (SM0) or 3 (SM3).
 * SM1/SM2 carry the audio module; other indices are
 * ignored.
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
 * Main application loop function called when a valid wireless packet
 * is received from the paired transmitter, typically at 50Hz frequency.
 *
 * Implement this function to map received channel values (via getChannel())
 * to motor and servo outputs (using setMotor() and setServo()).
 *
 * The function is called automatically by the firmware scaffold each time
 * a complete set of 16 channel values is received from the transmitter.
 * After loop() completes, the firmware updates all motor and servo outputs
 * based on the values set during this execution.
 */
void loop();

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

/**
 * Called when the RF link has been lost for longer than the debounce
 * window (MAIN_DISCONNECT_EVT fires after 400 ms without a packet).
 * Implement to put actuators into a safe state (e.g. zero motors,
 * center servos). Cancelled automatically if the link recovers before
 * the timer expires.
 */
void onDisconnect();

#endif
