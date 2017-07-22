#include "ioconfig.h"

void io_init() {
    // configure PS2 lines as inputs by default, internal pullups off
    PS2PORT &= ~(_BV(PS2CLK)|_BV(PS2DAT));  
    PS2DDR &= ~(_BV(PS2CLK)|_BV(PS2DAT));

	// jumper settings to inputs and pullups on
	OPTIONDDR &= ~((1<<OPTION_CLK) );
	OPTIONPORT |= ((1<<OPTION_CLK) );
	
	// LEDs to output turn on power
	LEDDDR |= ( (1<<LED_PWR) | (1<<LED_MODE) );
	LEDPORT |= (1<<LED_PWR);
	
	PORTD |= PORTD0;		// enable pullup up D0 UART RX
}

