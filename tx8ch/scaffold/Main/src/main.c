#include "HAL.h"
#include "pad.h"
#include "rf.h"
#include "ssd1306.h"
#include "uart1.h"
#include <stdio.h>
#include "app.h"

const static Gpio uartTx = {.portOut = &R32_PA_OUT, .pin = GPIO_Pin_9};
const static Gpio uartRx = {.portOut = &R32_PA_OUT, .pin = GPIO_Pin_8};

const static Gpio btnBoot = {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_22};

const static Gpio led = {.portOut = &R32_PA_OUT, .pin = GPIO_Pin_10};
const static Gpio buzzer = {.portOut = &R32_PA_OUT, .pin = GPIO_Pin_11};

SSDspi ssd = {
    .sclk = {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_13},
    .mosi = {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_14},
    .rst = {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_15},
    .dc = {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_12},
};

char ssdString[33];

#define CHANNEL_COUNT 8
static int8_t channels[CHANNEL_COUNT];

#define STICK_COUNT 6
static int8_t stickValues[STICK_COUNT];

static Stick sticks[STICK_COUNT] = {
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_12,
        .ain = CH_EXTIN_2,
        .offset = 0,
    },
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_13,
        .ain = CH_EXTIN_3,
        .offset = 0,
    },
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_14,
        .ain = CH_EXTIN_4,
        .offset = 0,
    },
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_15,
        .ain = CH_EXTIN_5,
        .offset = 0,
    },
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_4,
        .ain = CH_EXTIN_0,
        .offset = 0,
    },
    {
        .portOut = &R32_PA_OUT,
        .pin = GPIO_Pin_5,
        .ain = CH_EXTIN_1,
        .offset = 0,
    },
};

#define BUTTON_COUNT 4
uint8_t buttonValues;

const static Gpio buttons[BUTTON_COUNT] = {
    {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_4},  // Right
    {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_7},  // Left
    {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_10}, // Down
    {.portOut = &R32_PB_OUT, .pin = GPIO_Pin_11}, // Up
};

#define MAIN_ADC_EVT 1
#define MAIN_BUTTON_EVT 2

#define RF_TX_TX_EVT 1
#define RF_TX_HOP_EVT 2

tmosTaskID mainTaskID, rcTaskID;

static State state;

void loadMode(RFMode targetMode);

// rxBuf: [rssi, len, ...payload]
void rfTxCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf) {
  switch (sta) {
  case TX_MODE_RX_DATA:
    if (crc) {
    } else {
      if (state.modeCurrent == RFMODE_TX_PAIR) {
        if (rxBuf[1] == 9 && crc8smbus(rxBuf + 2, 8) == rxBuf[10]) {
          tmos_memcpy(state.preset.buffer, rxBuf + 2, 9);
          rfSavePreset(&state);
          loadMode(RFMODE_TX_TX);
        }
      } else {
        state.ackedCount++;
        uint8_t *pMsg = tmos_msg_allocate(rxBuf[1] + 2);
        tmos_memcpy(pMsg, rxBuf, rxBuf[1] + 2);
        tmos_msg_send(mainTaskID, pMsg);
      }
    }
    break;
  case TX_MODE_TX_FINISH:
  case TX_MODE_TX_FAIL:
  case TX_MODE_RX_TIMEOUT: // Timeout is about 200us
    break;
  case TX_MODE_HOP_SHUT: // connection lost
    gpioReset(&led);     // Turnoff Led if connection lost
    tmos_start_task(rcTaskID, RF_TX_HOP_EVT, MS1_TO_SYSTEM_TIME(20));
    break;
  }
}

void loadMode(RFMode targetMode) {
  gpioSet(&led);

  tmos_stop_task(rcTaskID, RF_TX_TX_EVT);
  tmos_stop_task(rcTaskID, RF_TX_HOP_EVT);

  ssdClear(&ssd, 0x00);

  state.ackedCount = 0;
  state.ackedCountLast = 0;

  if (targetMode == RFMODE_TX_TX) {
    rfSetup(state.me.address, state.preset.crcinit, rfTxCallBack);
    sprintf(ssdString, "addr:%08lx   crci:%08lx", state.me.address,
            state.preset.crcinit);

  } else if (targetMode == RFMODE_TX_PAIR) {
    rfSetup(RF_PAIR_ADDRESS, RF_PAIR_CRCINIT, rfTxCallBack);
    sprintf(ssdString, "TX Pairing...");
  }

  tmos_start_task(rcTaskID, RF_TX_HOP_EVT, MS1_TO_SYSTEM_TIME(20));

  state.modeCurrent = targetMode;

  ssdPutString(&ssd, ssdString, 0, 0);
  ssdRefresh(&ssd);
}

void pollBtnBoot() {
  static uint32_t pressedAt;
  static BOOL btnPressedLast = 0;
  static BOOL btnReleased = TRUE;

  BOOL btnPressed = !gpioRead(&btnBoot);

  if (btnPressed != btnPressedLast) {
    if (btnPressed) { // KeyDown
      btnReleased = FALSE;
      pressedAt = TMOS_GetSystemClock();

      if (state.modeCurrent == RFMODE_TX_PAIR && state.isPresetValid) {
        loadMode(RFMODE_TX_TX);
      }
    } else { // KeyUp
      btnReleased = TRUE;
    }
  }

  if (!btnReleased && TMOS_GetSystemClock() - pressedAt > 4800) { // 3 seconds
    btnReleased = TRUE; // soft release
    loadMode(RFMODE_TX_PAIR);
  }

  btnPressedLast = btnPressed;
}

BOOL getButton(uint8_t index) {
  index %= BUTTON_COUNT;
  return !(buttonValues & (1 << index));
}

int8_t getStick(uint8_t index) {
  index %= STICK_COUNT;
  return stickValues[index];
}

void setChannel(uint8_t index, int8_t value) {
  index &= CHANNEL_COUNT;
  channels[index] = value;
}

int8_t getChannel(uint8_t index) {
  return channels[index % CHANNEL_COUNT];
}

uint16_t MAIN_ProcessEvent(uint8_t task_id, uint16_t events) {
  static uint8_t msgAckRxed[32];

  if (events & SYS_EVENT_MSG) {
    uint8_t *pMsg = tmos_msg_receive(task_id);
    if (pMsg) {
      tmos_memcpy(msgAckRxed, pMsg, pMsg[1] + 2);
      tmos_msg_deallocate(pMsg); // Release the TMOS message
      for (uint8_t i = 0; i < pMsg[1] + 2; i++) {
        printf("%02x ", msgAckRxed[i]);
      }
      printf("\r\n");
    }
    return (events ^ SYS_EVENT_MSG);
  }

  if (events & MAIN_ADC_EVT) {
    tmos_start_task(task_id, MAIN_ADC_EVT, MS1_TO_SYSTEM_TIME(20));

    padReadAnalogs(stickValues, sticks, STICK_COUNT);
    buttonValues = padReadDigitals(buttons, BUTTON_COUNT);

    app();

    int8_t *msgChannels = tmos_msg_allocate(CHANNEL_COUNT);
    tmos_memcpy(msgChannels, channels, CHANNEL_COUNT);
    tmos_msg_send(rcTaskID, msgChannels);

    for (uint8_t i = 0; i < STICK_COUNT; i++) {
      sprintf(ssdString + i * 4, "%+4d", channels[i]);
      if (!channels[i])
        ssdString[i * 4 + 2] = ' '; // omit '+' for zeros
    }

    sprintf(ssdString + STICK_COUNT * 4, "%+4X", buttonValues);

    ssdClear(&ssd, 0x00);
    ssdPutString(&ssd, ssdString, 0, 0);
    ssdRefresh(&ssd);

    return (events ^ MAIN_ADC_EVT);
  }

  if (events & MAIN_BUTTON_EVT) {
    pollBtnBoot();
    tmos_start_task(task_id, MAIN_BUTTON_EVT, MS1_TO_SYSTEM_TIME(100));
    return events ^ MAIN_BUTTON_EVT;
  }

  return 0;
}

uint16_t RF_ProcessEvent(uint8_t task_id, uint16_t events) {
  static uint8_t packetTx[CHANNEL_COUNT + 1]; // [idx, ...channels]

  if (events & SYS_EVENT_MSG) {
    uint8_t *pMsg = tmos_msg_receive(task_id);
    if (pMsg) {
      tmos_memcpy(packetTx + 1, pMsg, CHANNEL_COUNT);
      tmos_msg_deallocate(pMsg);
    }
    return (events ^ SYS_EVENT_MSG);
  }
  if (events & RF_TX_TX_EVT) {
    RF_Shut();
    if (state.modeCurrent == RFMODE_TX_PAIR) {
      RF_Tx(state.me.buffer, 9, 0xFF, 0xFF);
    } else {
      packetTx[0]++;
      if (!(packetTx[0] & 0x3f)) {
        state.ackedCountLast = state.ackedCount;
        state.ackedCount = 0;
      }
      RF_Tx(packetTx, CHANNEL_COUNT + 1, 0xFF, 0xFF);
    }
    tmos_start_task(task_id, RF_TX_TX_EVT, MS1_TO_SYSTEM_TIME(20));
    return events ^ RF_TX_TX_EVT;
  }

  if (events & RF_TX_HOP_EVT) {
    gpioSet(&buzzer);
    if (RF_FrequencyHoppingTx(8)) {
      gpioReset(&buzzer);
      tmos_start_task(task_id, RF_TX_HOP_EVT, MS1_TO_SYSTEM_TIME(20));
    } else {
      gpioReset(&buzzer);
      gpioSet(&led); // Turnon Led if connection established
      tmos_start_task(task_id, RF_TX_TX_EVT, MS1_TO_SYSTEM_TIME(20));
    }
    return events ^ RF_TX_HOP_EVT;
  }
  return 0;
}

__HIGH_CODE
__attribute__((noinline)) void Main_Circulation() {
  while (1) {
    TMOS_SystemProcess();
  }
}

void main() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);

  uart1Init(&uartTx, &uartRx, 32, 32);

  gpioMode(&btnBoot, GPIO_ModeIN_PU);
  gpioMode(&led, GPIO_ModeOut_PP_5mA);
  gpioMode(&buzzer, GPIO_ModeOut_PP_5mA);

  tmos_memset(channels, 0, CHANNEL_COUNT);

  padInitAnalogPins(sticks, STICK_COUNT);
  padInitDigitalPins(buttons, BUTTON_COUNT);

  GPIOPinRemap(ENABLE, RB_PIN_SPI0);
  ssdInit(&ssd, 128, 32);

  CH57X_BLEInit();
  HAL_Init();

  RF_RoleInit();

  rcTaskID = TMOS_ProcessEventRegister(RF_ProcessEvent);

  mainTaskID = TMOS_ProcessEventRegister(MAIN_ProcessEvent);
  tmos_set_event(mainTaskID, MAIN_ADC_EVT);
  tmos_set_event(mainTaskID, MAIN_BUTTON_EVT);

  rfInit(&state);
  loadMode(state.modeCurrent);

  Main_Circulation();
}
