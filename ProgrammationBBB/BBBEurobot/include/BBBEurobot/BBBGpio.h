/// \file BBBGpio.h
/// \brief Functions concerning the Beaglebone GPIOs and serial ports.
///
/// \details This file implements all the functions to talk to the stepper motor controller.
/// 		 It is adapted from Sparkfun's driver for Arduino. All functions are thread-safe.
///	\note	 All functions in this header should be prefixed with gpio_.
#ifndef BBBGPIO_H
#define BBBGPIO_H

	#include <stdint.h>
	#include <glib.h>
	
	
	/// \brief Enable the Beaglebone serial ports.
	/// \details This function must be called at the start of the program, before any access to a
	///          serial port. This function returns nothing, but stops the program execution is uncessussful.
	///
	/// \warning This function uses the device tree overlay file "Eurobot-00A0.dtbo". This file must exist in 
	///          /lib/firmware, else this function fails. This function only activates this overlay : the behavior
	///          of the system entirely depends on this file being properly written.
	void gpio_enableSerialPorts();
#endif
