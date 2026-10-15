#ifndef __DEBUG_H
#define __DEBUG_H
#include "zf_common_headfile.h"

#define UART_INDEX              (DEBUG_UART_INDEX   )
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )

void mydebug_init(void);
void uart_rx_interrupt_handler(unsigned char dat);
void VOFA_ProcessByte(unsigned char rx_byte);
void debug_print_tuning_data(void);

#endif
