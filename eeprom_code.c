

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>

#include "eeprom_code.h"


extern uint8_t config[8];


const char ee_magic[8] = "[M]ouse!";
const char default_config[] PROGMEM = { 0, 0, 0, 0, 0, 0,0,0};

#define ARR_SIZEOF(x)  (sizeof(x) / sizeof(x[0]))

void check_eeprom(uint8_t force){
	char buf[20];
	uint8_t ee_invalid = 0;	// assume valid to start with

	eeprom_read_block((void *) buf, (void *) EE_ADDR_MAGIC, 8);
	if (memcmp(ee_magic, buf, 8) != 0) {
		ee_invalid = 1;
	}

	if ((ee_invalid == 1) || (force)) {
		eeprom_update_block ( ( const void *) ee_magic , ( void *) EE_ADDR_MAGIC, ARR_SIZEOF(ee_magic) );
		eeprom_update_block ( memcpy_P(buf, default_config,ARR_SIZEOF(default_config)) , ( void *) 0, ARR_SIZEOF(default_config) );
	}
	
	for (int i=0; i<6; i++) {
		config[i] = eeprom_read_byte((void*) i);		
	}	
}


void update_config(int addr) {
		eeprom_update_byte((void*) addr, config[addr]); 
}

void update_eeprom() {
	for (int i=0; i<6; i++) {
		eeprom_update_byte((void*) i, config[i]); 
	}	
}
