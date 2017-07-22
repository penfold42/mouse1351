///\file main.c
///\author Viacheslav Slavinsky
///
///\brief [M]ouse main file.
///
/// This is the main source file. The main loop inits usart, then ps2 functions, then c1351.
/// It then calls mouse_boot() which never exits until mouse is succesfully reset and 
/// initialized in streaming mode. Initial button status is reported and according to buttons
/// pressed at start, options are set. The default is to boot into C1351 proportional mode,
/// normal speed (2 counts per mm).
/// 
/// Right mouse button boots mouse in C1350 (Joystick) mode.
///
/// Left mouse button boots mouse in fast movement mode.
///
/// Middle mouse button boots mouse in slow mode.
///
/// A triple button chord at start enables VT-Paint doodle app that works in a VT220 terminal
/// attached to USART, if any, but probably affects performance of the C1351 mouse. This
/// is kept in for debugging and fun.
///
/// h/j/k/l/space keys in attached terminal can be used to simulate mouse movement.
///
/// \mainpage [M]ouse: PS/2 to Commodore C1351 Mouse Adapter
/// \section Description
/// [M]ouse lets you use a regular PS/2 mouse with a Commodore 64 computer. It supports
/// both proportional (analog, C1351) and joystick (C1350) modes. This is the source code
/// of [M]ouse firmware for ATmega8 microcontroller. It must be compiled with avr-gcc.
/// \section Files
/// - main.c    main file
/// - ps2.c     Interrupt-driven PS/2 protocol implementation
/// - mouse.c   Mouse protocol implementation: boot and configuration
/// - c1351.c   Timer-based Commodore mouse emulation
///
/// \section a How it works
/// It boots the PS/2 mouse into streaming mode. Mouse sends updated position with every
/// movement. This movement is translated into timer intervals. Every measurement cycle
/// that happens once in 512us, movement is loaded into output compare units of Timer1. 
/// When compare matches, corresponding output, POTX or POTY, is asserted high. Then
/// the cycle repeats.
/// 

//#define VTPAINT     ///< Compile VT-toy

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
#include "c1351.h"
#include "eeprom_code.h"

#if defined HAVE_PCJOY
#include "ADCpot.h"
	extern uint16_t Xpot_array[];	
	extern uint16_t Ypot_array[];	
#endif

/// Fresh movement packet, raw
MouseMovt   movt_packet;

/// Decoded movement packet
DecodedMovt movt;

int8_t mouse_type = -1;
uint8_t joymode = 0;
uint8_t speed = 0;
uint8_t config[8];

/// VT-Paint test app for VT220 doodling
void vtpaint();
 
/// Program main
int main() {
    uint8_t i;
    int8_t byte;
    uint8_t vtpaint_on = 0;
    
    uint16_t zero = 320;
	
	uint16_t loop_counter = 0;	// simple counter in the main loop

	uint16_t led_counter = 0;
	uint8_t last_buttons = 0;


		    
    io_init();

#if defined __AVR_ATmega328P__
	if (bit_is_clear(OPTIONPIN,OPTION_CLK)) {
		CLKPR = (1<<CLKPCE);					// enable register
		CLKPR = 1;	// div by 2 so 16 becomes 8MHz 
	} else {
		CLKPR = (1<<CLKPCE);					// enable register
		CLKPR = 0;	// div by 1 so 8 becomes 8MHz if they left the CKDIV8 fuse set!		
	}
#endif
	
    usart_init(F_CPU/16/19200-1);
	
//    printf_P(PSTR("\033[2J\033[H[M]AUS B%s (C)SVO 2009 PRESS @\n"), "1942");
    printf_P(PSTR("\033[2J\033[H[M]OUSE %s (C)SVO 2009 PRESS @\natmega328 port by penfold42\n"), __DATE__);

	check_eeprom(0);
	
    ps2_init();

 #if defined HAVE_PCJOY
  #define MAX_JOYMODE 3
	InitADC();	
#else 
  #define MAX_JOYMODE 2
#endif

    potmouse_init();
    potmouse_zero(zero);

    // enable interruptski
    sei();

    byte = mouse_boot();
    
	joymode = config[0];
	speed = config[1];

	if (byte != -1) {
		printf_P(PSTR("byte: %d\n"),byte);

		switch (byte & 7) {
			case 001: // [__@]
				if (++joymode >= MAX_JOYMODE) joymode = 0;
				break;
			case 004: // [@__]
				if (++speed >= 3) speed = 0;
				break;
		}
		mouse_setres(speed);
		printf_P(PSTR("Mouse speed: %d\n"),speed);
	} else {
		printf_P(PSTR("No PS/2 mouse found.\n"));
	}
	printf_P(PSTR("Joystick mode: %d\n"),joymode);
	
	// go update eeprom
	config[0] = joymode;
	config[1] = speed;
	update_config(0);
	update_config(1);

    potmouse_start(joymode);    
    potmouse_movt(0,0,0); 
    
    // usart seems to be capable of giving trouble when left disconnected
    // if no characters appear in buffer by this moment, disable it 
    // completely just in case
/*    
    if (!uart_available()) {
        usart_stop();
    } else if (uart_getchar() != '@') {
        usart_stop();
    }
*/
    printf_P(PSTR("hjkl to move, space = leftclick\n"));
    
    for(i = 0;;) {

		loop_counter++;
		if (loop_counter >= 8000) loop_counter = 0;
	
		if (bit_is_clear(LEDPORT,LED_PWR)) {
			led_counter++;
			if (led_counter > 4000) {
				LEDPORT |= (1<<LED_PWR);	// turn power led back on
				led_counter = 0;	
			}
		}
		if (joymode == POTMOUSE_C1351) {
			LEDPORT |= (1<<LED_MODE);
		} else {
			LEDPORT &= ~(1<<LED_MODE);
		}

		
  // handle PC joystick
 #if defined HAVE_PCJOY

		int joydx = 0;
		int joydy = 0;
		int joybuttons = 0;
		if (loop_counter%100 == 0) {
			process_pot_avg();
			if (Xpot_array[AVG_AVG] > Xpot_array[AVG_MID]*1.3) joydx = 1;
			if (Xpot_array[AVG_AVG] < Xpot_array[AVG_MID]/1.3) joydx = -1;
			if (Ypot_array[AVG_AVG] > Ypot_array[AVG_MID]*1.3) joydy = -1;
			if (Ypot_array[AVG_AVG] < Ypot_array[AVG_MID]/1.3) joydy = 1;
			( PCJOY_BUTPIN & _BV(PCJOY_BUT1) ) ? (joybuttons += 0) : (joybuttons += 1);
			( PCJOY_BUTPIN & _BV(PCJOY_BUT2) ) ? (joybuttons += 0) : (joybuttons += 2);
			if (joymode == 2) {
				potmouse_movt(joydx, joydy, joybuttons);
			}
		}
		if (loop_counter == 0) {
//			printf("pot %3d %3d %2d %2d\n", Xavg, Yavg, movt.dx, movt.dy);
			printf("Xpot %3d %3d %3d %3d\n", Xpot_array[AVG_AVG], Xpot_array[AVG_MIN], Xpot_array[AVG_MID], Xpot_array[AVG_MAX]);
//			printf("Xpot %3d %3d %2d %2d\n", joymode, joydx, joydy, joybuttons);
		}	
#endif 
  
		if (ps2_avail()) {
			byte = ps2_getbyte();
			movt_packet.byte[i] = byte;
			
			if (mouse_type == 0) { // normal PS2
				i = (i + 1) % 3;
			}
			if (mouse_type == 3) { // intellimouse
				i = (i + 1) % 4;
			}
			
			// parse full packet
			if (i == 0) {
				movt.dx = ((movt_packet.fields.bits & _BV(XSIGN)) ? 0xff00 : 0) | movt_packet.fields.dx;
				movt.dy = ((movt_packet.fields.bits & _BV(YSIGN)) ? 0xff00 : 0) | movt_packet.fields.dy;
				movt.dz = ((movt_packet.fields.dz & _BV(3)) ? 0xfff0 : 0) | movt_packet.fields.dz;
				
				if (movt.dz != 0) {
					printf_P(PSTR("Z: %d\n"), movt.dz);						
				}
				movt.buttons = movt_packet.fields.bits & 7;
				
				// tell c1351 emulator that movement happened
				if (joymode <= 1) potmouse_movt(movt.dx, movt.dy, movt.buttons);

				// doodle on vt terminal
				if (vtpaint_on) vtpaint();
			}
		}


// hold middle, hit right 					4 then 4+2 
		if ( (last_buttons == 4) && (movt.buttons == 6)) {
			if (++joymode >= MAX_JOYMODE) joymode = 0;

			potmouse_start(joymode);
			potmouse_movt(0,0,0);
			printf_P(PSTR("joymode %d\n"), joymode);
		}
		if ( (last_buttons == 4) && (movt.buttons == 5)) {
			if (++speed >= 3) speed = 0;
			mouse_setres(speed);
			printf_P(PSTR("Mouse speed: %d\n"),speed);
		}

// hold either joystick button, press left mouse button
#ifdef HAVE_PCJOY
		if ( (last_buttons == 0) && (movt.buttons == 1)) {
			if ((PCJOY_BUTPIN & PCJOY_BUTMASK) != PCJOY_BUTMASK ) {	// either joystick button active
				printf_P(PSTR("Write joy\n"),speed);
				config[EE_ADDR_XMIN] =	Xpot_array[AVG_MIN];
				config[EE_ADDR_XMAX] =	Xpot_array[AVG_MAX];
				config[EE_ADDR_YMIN] =	Ypot_array[AVG_MIN];
				config[EE_ADDR_YMAX] = Ypot_array[AVG_MAX];
 
				update_config(2);
				update_config(3);
				update_config(4);
				update_config(5);
			}
		}

#endif
		last_buttons = movt.buttons;


#ifdef VTPAINT
        // handle keyboard commands
        if (uart_available()) {
            putchar(byte = uart_getchar());
            switch (byte) {             
                case 'q':   potmouse_zero(--zero);
                            break;       
                case 'w':   potmouse_zero(++zero);
                            break;
                case 'h':   potmouse_movt(-1, 0, 0);
                            break;
                case 'l':   potmouse_movt(1, 0, 0);
                            break;
                case 'j':   potmouse_movt(0, -1, 0);
                            break;
                case 'k':   potmouse_movt(0, 1, 0);
                            break;
                case ' ':   potmouse_movt(0, 0, 1);
                            break;
            }
        }
#endif
    }
}

void vtpaint() {                    
#ifdef VTPAINT
    static int16_t absoluteX = 0, absoluteY = 0;
    static int16_t termX, termY;
    static uint8_t last_buttons = 0;

    uint8_t moved = 0;

    ps2_enable_recv(0);
    absoluteX += movt.dx;
    absoluteY += movt.dy;

    moved = termX != absoluteX/33 || termY != absoluteY/66 || last_buttons != movt.buttons;

    if (moved) {
        printf_P(PSTR("\033[%d;%dH%c"), 12-termY, 40+termX, movt.buttons ? '#':' ');

        last_buttons = movt.buttons;
        termX = absoluteX/33;
        termY = absoluteY/66;

        printf_P(PSTR("\033[%d;%dH%c"), 12-termY, 40+termX, movt.buttons + '0');
    }

    printf_P(PSTR("\033[HX=%+6d Y=%+6d [%c %c %c]\r"), 
                absoluteX, absoluteY,
                (movt.buttons & _BV(BUTTON1)) ? '@':' ',
                (movt.buttons & _BV(BUTTON3)) ? '@':' ',
                (movt.buttons & _BV(BUTTON2)) ? '@':' ');
    
    ps2_enable_recv(1);
#endif
}


