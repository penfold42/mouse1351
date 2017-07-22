
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "ADCpot.h"
#include "ioconfig.h"
#include "eeprom_code.h"

uint16_t Xpot_array[AVG_SAMPLES+5];	// 0..AVG_MAX-1 is data, then min, max, mid
uint16_t Ypot_array[AVG_SAMPLES+5];

extern uint8_t config[8];

int pot_avg_valid = 0;
int j=0;

#if defined HAVE_PCJOY
void InitADC()
{
	
	// need these pot power lines for other things now...
	//	DDRC	= (1<<POT_VCC) | (1<<POT_GND);
	//	PORTC	= (1<<POT_VCC);
	DIDR0	= ((1<<PCJOY_XAXIS)|(1<<PCJOY_YAXIS));	// disable digital input
	// Select Vref=AVcc
	ADMUX = (1<<REFS0) | (PCJOY_XAXIS & 0x0F);

	#define ADC_PRESCALE fast_int_log2(F_CPU/125000)
	
 
 // single conversions
	ADCSRA = (1<<ADEN) | (0<<ADATE) | (ADC_PRESCALE&0x7) ;
	ADCSRB = (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

	PCJOY_BUTDDR &= ~(_BV(PCJOY_BUT1) | _BV(PCJOY_BUT2) );	// PC joystick buttons to inputs
	PCJOY_BUTPORT |= (_BV(PCJOY_BUT1) | _BV(PCJOY_BUT2) );	// PC joystick buttons with pullups
	Xpot_array[AVG_MIN] = config[EE_ADDR_XMIN];
	Xpot_array[AVG_MAX] = config[EE_ADDR_XMAX];
	Ypot_array[AVG_MIN] = config[EE_ADDR_YMIN];
	Ypot_array[AVG_MAX] = config[EE_ADDR_YMAX];

	if (PCJOY_BUTPIN & (_BV(PCJOY_BUT1) | _BV(PCJOY_BUT2) ) ) {	// PC joystick buttons to inputs
		Xpot_array[AVG_MIN] = config[EE_ADDR_XMIN];
		Xpot_array[AVG_MAX] = config[EE_ADDR_XMAX];
		Ypot_array[AVG_MIN] = config[EE_ADDR_YMIN];
		Ypot_array[AVG_MAX] = config[EE_ADDR_YMAX];
	} else {
		printf_P(PSTR("Joy Ignore Config\n"));
		Xpot_array[AVG_MIN] = 1023;
		Xpot_array[AVG_MAX] = 0;
		Ypot_array[AVG_MIN] = 1023;
		Ypot_array[AVG_MAX] = 0;
	}

}

uint16_t ReadADC()
{
	// Start conversion
	ADCSRA |= (1<<ADSC);
	// wait until ADC conversion is complete
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}


uint16_t process_pot_readings(uint16_t* which_pot, int which_adc) {
	ADMUX = (1<<REFS0) | (which_adc & 0x0F);
	which_pot[which_pot[AVG_CTR]++] = ReadADC();
	if (which_pot[AVG_CTR] >= AVG_SAMPLES) {
		which_pot[AVG_CTR] = 0;
	}
		
	uint16_t avg = 0;
	for (j=0; j<AVG_SAMPLES; j++) {
		avg += which_pot[j];		
	}
		
	avg /= AVG_SAMPLES;	
	avg = 10230/avg+10;

	which_pot[AVG_AVG] = avg;
	if (pot_avg_valid) {
		if (avg > which_pot[AVG_MAX]) which_pot[AVG_MAX] = avg;
		if (avg < which_pot[AVG_MIN]) which_pot[AVG_MIN] = avg;
		which_pot[AVG_MID] = (which_pot[AVG_MIN]+which_pot[AVG_MAX])/2;
	}

	return 42;
}


uint8_t fast_int_log2(uint8_t x) {
	uint8_t y = 0;
	while(x >= 2) {
		y++;
		x >>= 1;
	}
	return y;
}

int pot_state_ctr;

uint16_t process_pot_avg() {

	if (pot_state_ctr++ >= AVG_SAMPLES*2) {
		pot_state_ctr = 0;
		pot_avg_valid = 1;
	}
	if ((pot_state_ctr >= 0) || (pot_state_ctr <AVG_SAMPLES)) {
		process_pot_readings(Xpot_array, PCJOY_XAXIS);
	}
	if ((pot_state_ctr >= AVG_SAMPLES) || (pot_state_ctr < AVG_SAMPLES*2)) {
		process_pot_readings(Ypot_array, PCJOY_YAXIS);
	}
	return 0;
}
#endif
