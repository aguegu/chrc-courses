#include "uart1.h"

static RingBuffer txBuffer, rxBuffer;

void uart1Init(const Gpio * tx, const Gpio * rx, uint8_t txLen, uint8_t rxLen) {
  ringbufferInit(&txBuffer, txLen);
  ringbufferInit(&rxBuffer, rxLen);

  gpioSet(tx);
  gpioMode(rx, GPIO_ModeIN_PU);
  gpioMode(tx, GPIO_ModeOut_PP_5mA);

  UART1_DefInit();
  UART1_ByteTrigCfg(UART_7BYTE_TRIG);
  UART1_INTCfg(ENABLE, RB_IER_THR_EMPTY | RB_IER_RECV_RDY);
  PFIC_EnableIRQ(UART1_IRQn);
}

void uart1FlushTx() {
  while (!(R8_UART1_LSR & RB_LSR_TX_FIFO_EMP)) {
    __WFI();
  }
  while (!(R8_UART1_LSR & STA_TXALL_EMP))
    ;
}

int _write(int fd, char *buf, int size) {
  for (int i = 0; i < size; i++) {
    ringbufferPut(&txBuffer, *buf++, TRUE);
    if (R8_UART1_LSR & RB_LSR_TX_FIFO_EMP) {
      while (ringbufferAvailable(&txBuffer) && R8_UART1_TFC < UART_FIFO_SIZE) {
        R8_UART1_THR = ringbufferGet(&txBuffer);
      }
    }
  }
  return size;
}

BOOL uart1RxAvailable() { return ringbufferAvailable(&rxBuffer); }

uint8_t uart1RxGet() { return ringbufferGet(&rxBuffer); }

__HIGH_CODE
__INTERRUPT
void UART1_IRQHandler(void) {
  switch (UART1_GetITFlag()) {
  case UART_II_THR_EMPTY: // trigger when THR and FIFOtx all empty
    while (ringbufferAvailable(&txBuffer) && R8_UART1_TFC != UART_FIFO_SIZE) {
      R8_UART1_THR = ringbufferGet(&txBuffer);
    }
    break;
  case UART_II_RECV_RDY:  // Rx FIFO is full
  case UART_II_RECV_TOUT: // Rx FIFO is not full, but there is something when no
                          // new data comming in within timeout
    while (R8_UART1_RFC) {
      ringbufferPut(&rxBuffer, R8_UART1_RBR, FALSE);
    }
    break;
  }
}
