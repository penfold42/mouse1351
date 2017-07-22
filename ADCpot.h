/*
 * adcstuff.h
 *
 * Created: 14/12/2014 1:46:19 PM
 *  Author: u492869
 */ 


#ifndef ADCSTUFF_H_
#define ADCSTUFF_H_

#define AVG_SAMPLES		8		// how many ADC samples do we average?
#define AVG_MIN AVG_SAMPLES
#define AVG_MID AVG_SAMPLES+1
#define AVG_MAX AVG_SAMPLES+2
#define AVG_CTR AVG_SAMPLES+3
#define AVG_AVG AVG_SAMPLES+4

void InitADC();
uint16_t ReadADC(void);
void process_pot (void);
uint16_t process_pot_avg (void);

uint8_t fast_int_log2(uint8_t x);




#endif /* ADCSTUFF_H_ */
