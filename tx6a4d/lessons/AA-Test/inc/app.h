#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

bool getButton(uint8_t index);

int8_t getStick(uint8_t index);

void setChannel(uint8_t index, int8_t value);

int8_t getChannel(uint8_t index);

void loop();

#endif
