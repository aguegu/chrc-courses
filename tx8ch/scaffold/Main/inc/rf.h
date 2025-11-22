#ifndef RF_H
#define RF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#define STORAGE (0x7e00)
#define RF_PAIR_ADDRESS  0xF0F0F0F0
#define RF_PAIR_CRCINIT  0

typedef enum {
  RFMODE_TX_TX,
  RFMODE_TX_PAIR,
} RFMode;

typedef struct {
  RFMode modeCurrent;
  uint8_t ackedCount;
  uint8_t ackedCountLast;
  BOOL isPresetValid;
  union {
    struct {
      uint32_t batch;
      uint32_t address;
    };
    uint8_t buffer[9];
  } me;
  union {
    struct {
      uint32_t batch;
      uint32_t crcinit;
    };
    uint8_t buffer[9];
  } preset;
} State;

void rfSetup(uint32_t address, uint32_t crcinit, pfnRFStatusCB_t cb);

uint8_t crc8smbus(const uint8_t *data, uint8_t len);

void rfInit(State * p);
void rfSavePreset(State *p);

#ifdef __cplusplus
}
#endif

#endif
