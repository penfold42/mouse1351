//! \file
//! \brief USART interface
#ifndef _USRAT_H
#define _USRAT_H

#define RX_BUFFER_SIZE	4					//!< USART RX buffer length

void usart_init(uint16_t baudrate);
void usart_stop();

int uart_putchar(char data, FILE* stream);
int uart_getchar();
uint8_t uart_available(void);
uint8_t uart_getc();

#endif

// $Id$
