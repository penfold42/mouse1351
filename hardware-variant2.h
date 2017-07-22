/*
 * hardware_variant1.h
 *
 * Created: 1/02/2015 11:17:48 AM
 * pin assignments for penfold42's tic tac box with atmega328
 */ 


#ifndef HARDWARE_VARIANT2_H_
#define HARDWARE_VARIANT2_H_

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

#define JOYFIREPORT     PORTC       ///< Joystick pins
#define JOYFIREDDR      DDRC        ///< Joystick pins data direction (out = switch closed to gnd)
#define JOYFIREPIN      PINC        ///< Joystick in

#define JOYUP       PORTC5           ///< Joystick UP switch
#define JOYDOWN     PORTC4           ///< Joystick DOWN switch
#define JOYLEFT     PORTC3           ///< Joystick LEFT switch
#define JOYRIGHT    PORTC2           ///< Joystick RIGHT switch
#define JOYFIRE     PORTC1           ///< Joystick FIRE switch

#define LEDPORT     PORTB       ///<	LEDs are active HIGH
#define LEDDDR      DDRB        ///<
#define LEDPIN      PINB        ///<
#define LED_PWR		PORTB5           ///<
#define LED_MODE	PORTB4           ///<

#define OPTIONPORT     PORTD       ///<
#define OPTIONDDR      DDRD        ///<
#define OPTIONPIN      PIND        ///<
#define OPTION_CLK		PORTD5           ///<	tie low for a 16 MHz clock

#undef HAVE_PCJOY

#endif /* HARDWARE_VARIANT2_H_ */
