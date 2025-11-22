#include "pad.h"

void padInitAnalogPins(Stick *sticks, uint8_t len) {
  ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_1_4);

  for (uint8_t i = 0; i < len; i++) {
    Gpio gpio = {.portOut = sticks[i].portOut, .pin = sticks[i].pin};
    gpioMode(&gpio, GPIO_ModeIN_Floating);
  }
}

void padInitDigitalPins(const Gpio *pins, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    gpioMode(pins + i, GPIO_ModeIN_PU);
  }
}

int8_t adcToAxe(uint16_t adc) {
  int16_t raw = adc / 6.2f - 376;

  if (raw < -127 + DEADZONE) {
    raw = -127;
  } else if (raw > 127 - DEADZONE) {
    raw = 127;
  } else if (raw < DEADZONE && raw > -DEADZONE) {
    raw = 0;
  }
  return (int8_t)raw;
}

void padReadAnalogs(int8_t *p, Stick *sticks, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    ADC_ChannelCfg(sticks[i].ain);
    p[i] = adcToAxe(ADC_ExcutSingleConver() - sticks[i].offset);
  }
}

uint8_t padReadDigitals(const Gpio *buttons, uint8_t len) {
  uint8_t btns = 0;
  for (uint8_t i = 0; i < len; i++) {
    btns |= gpioRead(buttons + i) ? (1 << i) : 0;
  }
  return btns;
}
