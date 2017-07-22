/*
 * eeprom.h
 *
 * Created: 17/01/2015 5:07:42 PM
 *  Author: u492869
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void check_eeprom(uint8_t force);
void update_eeprom();
void update_config(int addr);

#define EE_ADDR_JOYMODE		0
#define EE_ADDR_SPEED		1
#define EE_ADDR_XMIN		2
#define EE_ADDR_XMAX		3
#define EE_ADDR_YMIN		4
#define EE_ADDR_YMAX		5



#define EE_ADDR_MAGIC	480



#endif /* EEPROM_H_ */
