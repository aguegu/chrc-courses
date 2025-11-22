#ifndef UART1_H
#define UART1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CH57x_common.h"
#include "ringbuffer.h"
#include "gpio.h"

#ifndef __HIGH_CODE
#define __HIGH_CODE __attribute__((section(".highcode")))
#endif

#ifndef __INTERRUPT
#define __INTERRUPT __attribute__((interrupt("WCH-Interrupt-fast")))
#endif

void uart1Init(const Gpio * tx, const Gpio * rx, uint8_t txLen, uint8_t rxLen);

void uart1FlushTx();

BOOL uart1RxAvailable();

uint8_t uart1RxGet();

#ifdef __cplusplus
}
#endif

#endif
