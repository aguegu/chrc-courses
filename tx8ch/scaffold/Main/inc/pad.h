#ifndef PAD_H
#define PAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"

typedef struct {
  uint16_t pin;
  PUINT32V portOut;
  ADC_SingleChannelTypeDef ain;
  int16_t offset;
} Stick;

void padInitAnalogPins(Stick * sticks, uint8_t len);

int8_t adcToAxe(uint16_t adc);
void padInitDigitalPins(const Gpio * pins, uint8_t len);

void padReadAnalogs(int8_t *p, Stick * stick, uint8_t len);
uint8_t padReadDigitals(const Gpio * buttons, uint8_t len);

#define DEADZONE (3)

#ifdef __cplusplus
}
#endif

#endif
