/*
 * hardware_variant3.h
 *
 * Created: 1/02/2015 11:17:48 AM
 * pin assignments for later!
 */ 


#ifndef HARDWARE_VARIANT3_H_
#define HARDWARE_VARIANT3_H_

#define PS2PORT PORTD           ///< PS2 port
#define PS2PIN  PIND            ///< PS2 input
#define PS2DDR  DDRD            ///< PS2 data direction
#define PS2CLK  PORTD2               ///< PS2CLK is pin 2
#define PS2DAT  PORTD4               ///< PS2DAT is pin 4


#define SENSEPORT   PORTD       ///< SID sense port
#define SENSEDDR    DDRD        ///< SID sense data direction
#define SENSEPIN    PIND        ///< SID sense input
#define POTSENSE    PORTD3           ///< INT1 attached to PORTD.3,

#define POTPORT     PORTB       ///< POT-controlling outputs X and Y
#define POTDDR      DDRB        ///< POT outputs data direction
#define POTPIN      PINB        ///< POT outputs input ;)
#define POTY        PORTB1           ///< Y-line, also OC1A
#define POTX        PORTB2           ///< X-line, also OC1B, also right button in joystick mode

#define JOYPORT     PORTC       ///< Joystick pins
#define JOYDDR      DDRC        ///< Joystick pins data direction (out = switch closed to gnd)
#define JOYPIN      PINC        ///< Joystick in

#define JOYFIREPORT     PORTB       ///< Joystick pins
#define JOYFIREDDR      DDRB        ///< Joystick pins data direction (out = switch closed to gnd)
#define JOYFIREPIN      PINB        ///< Joystick in

#define JOYUP       PORTC5           ///< Joystick UP switch
#define JOYDOWN     PORTC4           ///< Joystick DOWN switch
#define JOYLEFT     PORTC3           ///< Joystick LEFT switch
#define JOYRIGHT    PORTC2           ///< Joystick RIGHT switch
#define JOYFIRE     PORTB0           ///< Joystick FIRE switch

#define LEDPORT     PORTB       ///<	LEDs are active HIGH
#define LEDDDR      DDRB        ///<
#define LEDPIN      PINB        ///<
#define LED_PWR		PORTB5           ///<
#define LED_MODE	PORTB4           ///<

#define OPTIONPORT     PORTD       ///<
#define OPTIONDDR      DDRD        ///<
#define OPTIONPIN      PIND        ///<
#define OPTION_CLK		PORTD5           ///<	tie low for a 16 MHz clock

#define PCJOY_BUTPORT	PORTD
#define PCJOY_BUTDDR	DDRD
#define PCJOY_BUTPIN	PIND
#define PCJOY_BUT1		PORTD6
#define PCJOY_BUT2		PORTD7
#define PCJOY_BUTMASK	(_BV(PCJOY_BUT1) | _BV(PCJOY_BUT2) )

#define PCJOY_POTPORT	PORTC
#define PCJOY_POTDDRl	DDRC
#define PCJOY_XAXIS		PORTC0
#define PCJOY_YAXIS		PORTC1

#define HAVE_PCJOY

#endif /* HARDWARE_VARIANT3_H_ */
