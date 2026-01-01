#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>
#include "neo.h"

int8_t getChannel(uint8_t index);

void setMotor(uint8_t index, int8_t value);
int8_t getMotor(uint8_t index);


void setServo(uint8_t index, uint8_t value);

void setup();
void loop();

void neo();

#endif
