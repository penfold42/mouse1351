//!\file 
//!\brief Mouse protocol implementation.


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "usrat.h"
#include "ioconfig.h"
#include "ps2.h"
#include "mouse.h"
#include <util/delay.h>

const char PSTR_OK[]    PROGMEM      = "OK";
const char PSTR_ERROR[] PROGMEM      = "ERROR";

extern int8_t mouse_type;

static void mouse_flush(uint8_t pace) {
//        tdelay(pace);
    for (int i=0; i<pace;i++) _delay_ms(1); 
    do {
        if (ps2_avail()) printf_P(PSTR("%02x "), ps2_getbyte());
//        tdelay(pace);
	    for (int i=0; i<pace;i++) _delay_ms(1);
    } while (ps2_avail());
}

int8_t mouse_reset() {
    uint8_t i, b = 33;
    const int ntries = 4;

    // send reset command    
    ps2_sendbyte(MOUSE_RESET);
    ps2_sendbyte(MOUSE_RESET);
    ps2_sendbyte(MOUSE_RESET);
    
    // wait for some time for mouse self-test to complete
    for (i = 0; i < ntries; i++) {
//        tdelay(250);
		_delay_ms(250);
        if (ps2_avail()) {
            b = ps2_getbyte(); printf_P(PSTR("%02x "),b);
            if (b == MOUSE_RESETOK) {
                break;
            } else if (b == MOUSE_ACK){
				// do nothing, just swallow it
			} else {
                i = ntries;
                break;
            }
        }
    }

    if (i == ntries) return -1;
    
    // flush the rest of reponse, most likely mouse id == 0
//    tdelay(100);
    _delay_ms(100);
    mouse_flush(0);
    
    return 0;
}

int16_t mouse_command(uint8_t cmd, uint8_t wait) {
    int16_t response = -1;
    
    ps2_sendbyte(cmd);
    if (wait) {
//        tdelay(22);
        _delay_ms(22);
        if (ps2_avail()) response = ps2_getbyte();
    }
    
    printf_P(PSTR("%02x>%02x "), cmd, response); 
    
    return response;
}


void mouse_setres(uint8_t res) {
    mouse_command(MOUSE_DDR,1);
    
    mouse_command(MOUSE_SETRES, 1);
    mouse_command(res, 1);             // 0 = 1, 1 = 2, 2 = 4, 3 = 8 counts/mm
    
    mouse_command(MOUSE_EDR,1);
	printf("\n");
}


int8_t mouse_boot() {
	int found_mouse = 0;
    uint8_t buttons = 0;
	    
    ps2_enable_recv(1);

    for(;;) {
        printf_P(PSTR("\nRESET: "));
		found_mouse = mouse_reset();
        if (found_mouse == 0) {
            puts_P(PSTR_OK);
            break;
        } else {
            puts_P(PSTR_ERROR);
			return -1;
        }
    }

#define TRY_INTELLIMOUSE
#ifdef TRY_INTELLIMOUSE

    mouse_command(MOUSE_SSR, 1);
	mouse_command(200,1);
    mouse_command(MOUSE_SSR, 1);
    mouse_command(100,1);
    mouse_command(MOUSE_SSR, 1);
    mouse_command(80,1);
#endif

    mouse_command(MOUSE_GETID, 1);
//    tdelay(22);
    _delay_ms(22);
    if (ps2_avail()) mouse_type = ps2_getbyte();
    printf_P(PSTR("T:%x\n"), mouse_type);

    mouse_command(MOUSE_DDR, 1);
    mouse_command(MOUSE_SETSCALE21, 1);
    
    mouse_command(MOUSE_SETRES, 1);
    mouse_command(1,1);             // 0 = 1, 1 = 2, 2 = 4, 3 = 8 counts/mm


    mouse_command(MOUSE_STATUSRQ, 1);
//    tdelay(22);
	_delay_ms(22);
    if (ps2_avail()) buttons = ps2_getbyte();
    
    mouse_flush(22);
    
    printf_P(PSTR("B:%x\n"), buttons);

    mouse_command(MOUSE_EDR, 1);

    mouse_flush(100);

    printf_P(PSTR("\n"));
    
    return buttons&7;    
}

