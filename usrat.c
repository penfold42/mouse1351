//! \file
//! \brief USART interface

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "usrat.h"

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint8_t rx_buffer_in;
static volatile uint8_t rx_buffer_out;

//! \brief a stub to use when usart is disabled
static int uart_non(char data, FILE* stream) {
	return 0;
}

//! \brief Initialize USART, perform fdevopen with uart_putchar.
//! \param baudval (F_CPU/(16*baudrate))-1
//! \sa uart_putchar()
void usart_init(uint16_t baudval) {
	// Set baud rate

#if defined __AVR_ATmega8__
	UBRRH = (uint8_t)(baudval>>8);
	UBRRL = (uint8_t)baudval;
#elif defined __AVR_ATmega328P__
	UBRR0H = (uint8_t)(baudval>>8);
	UBRR0L = (uint8_t)baudval;
#else
# error unknown chip
#endif

	rx_buffer_in = rx_buffer_out = 0;

#if defined __AVR_ATmega8__
	// Set frame format: 8 data, 1 stop bit
	UCSRC = (uint8_t)((1<<URSEL) | (0<<USBS) | (3<<UCSZ0));
	
	// Enable receiver and transmitter, enable RX complete interrupt
	UCSRB = (uint8_t)((1<<RXEN) | (1<<TXEN) | (1<<RXCIE));
#elif defined __AVR_ATmega328P__

	UCSR0C = (uint8_t)((0<<USBS0) | (1<<UCSZ01)| (1<<UCSZ00));
	UCSR0B = (uint8_t)((1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0));
#else
# error unknown chip
#endif

//	(void)fdevopen(uart_putchar, NULL/*, 0*/);
	(void)fdevopen(uart_putchar, NULL);
}

//! \brief Disable USART completely
void usart_stop() {
#if defined __AVR_ATmega8__
    UCSRB = 0;
#elif defined __AVR_ATmega328P__
    UCSR0B = 0;
#else
# error unknown chip
#endif
    (void)fdevopen(uart_non,NULL);
}

//! \brief putchar() for USART.
//! \param data character to print.
int uart_putchar(char data, FILE* stream) {
	//while (!(UCSRA & (1<<UDRE))) {};
	if (data == '\n') {
		(void)uart_putchar('\r', NULL);
	}

	//PORTD |= 1<<5;
#if defined __AVR_ATmega8__
	while (!(UCSRA & (1<<UDRE))) {};
#elif defined __AVR_ATmega328P__
	while (!(UCSR0A & (1<<UDRE0))) {};
#else
# error unknown chip
#endif
	//PORTD &= (uint8_t)(~(1<<5));

#if defined __AVR_ATmega8__
	UDR = (uint8_t)data;
#elif defined __AVR_ATmega328P__
	UDR0 = (uint8_t)data;
#else
# error unknown chip
#endif

	return 0;
}

//! \brief getchar() for USART. Wait for data if not available.
//! \return value read.
//! \sa uart_available()
int uart_getchar() {
	while (!uart_available());
	
	return (int) uart_getc();
}

//! \brief Check data availability in USART buffer.
//! \return 1 if buffer is not empty.
uint8_t uart_available() {
	return rx_buffer_in != rx_buffer_out;
}

//! \brief Nonblocking, nonchecking getchar for USART. Use with care.
uint8_t uart_getc() {
	uint8_t result = rx_buffer[rx_buffer_out];
	rx_buffer_out = (rx_buffer_out + 1) % RX_BUFFER_SIZE;
	
	return result;
}


#if defined __AVR_ATmega8__

void USART_RXC_vect( void ) __attribute__ ( ( signal ) );
void USART_RXC_vect( void ) {
	rx_buffer[rx_buffer_in] = (uint8_t)UDR;
	rx_buffer_in = (rx_buffer_in + 1) % RX_BUFFER_SIZE;
}

#elif defined __AVR_ATmega328P__

void USART_RX_vect ( void ) __attribute__ ( ( signal ) );  
void USART_RX_vect ( void ) {
	rx_buffer[rx_buffer_in] = (uint8_t)UDR0;
	rx_buffer_in = (rx_buffer_in + 1) % RX_BUFFER_SIZE;
}

#else
# error unknown chip
#endif


