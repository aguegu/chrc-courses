#ifndef APP_H
#define APP_H

#include <stdint.h>

int8_t getChannel(uint8_t index);

void setMotor(uint8_t index, int8_t value);
void setServo(uint8_t index, uint8_t value);

void loop();

#endif
