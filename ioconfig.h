///\file ioconfig.h
///\brief I/O port definitions

#ifndef _IOCONFIG_H
#define _IOCONFIG_H

#include <avr/io.h>

#define PS2_RXBUF_LEN  16       ///< PS2 receive buffer size

#if CONFIG_HARDWARE_VARIANT==1
	#include "hardware-variant1.h"
#elif CONFIG_HARDWARE_VARIANT==2
	#include "hardware-variant2.h"
#elif CONFIG_HARDWARE_VARIANT==3
	#include "hardware-variant3.h"
#else
	# error invalid CONFIG_HARDWARE_VARIANT
#endif

void io_init();

#endif

