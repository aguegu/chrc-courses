#include "rf.h"

__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

void rfSetup(uint32_t address, uint32_t crcinit, pfnRFStatusCB_t cb) {
  rfConfig_t rfConfig;

  tmos_memset(&rfConfig, 0, sizeof(rfConfig_t));

  rfConfig.accessAddress = address;
  rfConfig.CRCInit = crcinit;

  rfConfig.ChannelMap = 0xFFFFFFFF;

  rfConfig.HeartPeriod = 0x10;
  rfConfig.HopPeriod = 8;
  rfConfig.HopIndex = 17;

  // rfConfig.accessAddress = 0x71764129; // 禁止使用0x55555555以及0xAAAAAAAA
  // (建议不超过24次位反转，且不超过连续的6个0或1 ) rfConfig.CRCInit = 0x555555;

  rfConfig.LLEMode = LLE_MODE_AUTO;
  rfConfig.rfStatusCB = cb;
  rfConfig.RxMaxlen = 31;
  rfConfig.TxMaxlen = 31;

  RF_Config(&rfConfig);
}

uint8_t crc8smbus(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++)
      crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
  }
  return crc;
}

void rfInit(State *p) {
  p->ackedCount = 0;
  p->ackedCountLast = 0;

  FLASH_EEPROM_CMD(CMD_GET_UNIQUE_ID, 0, p->me.buffer, 8);
  p->me.buffer[8] = crc8smbus(p->me.buffer, 8);

  EEPROM_READ(STORAGE, p->preset.buffer, 9);

  p->isPresetValid = crc8smbus(p->preset.buffer, 8) == p->preset.buffer[8];

  p->modeCurrent = p->isPresetValid ? RFMODE_TX_TX : RFMODE_TX_PAIR;
};

void rfSavePreset(State *p) { EEPROM_WRITE(STORAGE, p->preset.buffer, 9); }
