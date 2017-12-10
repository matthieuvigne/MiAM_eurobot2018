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


	/// \brief Enable the Beaglebone serial ports and gpio.
	/// \details This function must be called at the start of the program, before any access to a
	///          serial port or gpio. This function returns nothing, but stops the program execution is uncessussful.
	///
	/// \warning This function uses the device tree overlay file "Eurobot-00A0.dtbo". This file must exist in
	///          /lib/firmware, else this function fails. This function only activates this overlay : the behavior
	///          of the system entirely depends on this file being properly written.
	void gpio_enablePorts();


	/// \brief Export a gpio pin, setting the direction.
	/// \details Exporting a gpio pin maks it available in user-space. In practice, this create a folder in
	///			 /sys/class/gpio/gpioX, where X is the gpio number. However, this does not necessarily give control
	///			 of the pin : this is done at pinmux level in the overlay (see Eurobot overlay). As such, if the pin
	///			 is used for something else (serial communication for example), an operation done on the gpio is meaningless.
	///			 Note also that, while exporting the gpio does set its direction, there is no way in user space to
	///			 change the pullups on the pin.
	///			 This function never fails, but one again, it is up to the user to check that the pin is configured correctly.
	///
	/// \param[in] pin gpio pin number (kernel representation).
	/// \param[in] direction gpio direction, either "in" or "out"
	///
	/// \return 0 on success, -1 on file access error.
	int gpio_exportPin(int pin, gchar *direction);

	/// \brief Read a digital input.
	/// \details The pin must be configured as an input for the return value to make sense.
	///
	/// \param[in] pin gpio pin number (kernel representation).
	///
	/// \return Positive value on success : 0 for low, 1 for high.
	///			Negative value on failure: -1: reading error (probably pin not enabled). -2: pin enabled but not an input.
	int gpio_digitalRead(int pin);


	/// \brief Write to a digital output.
	/// \details The pin must be configured as an input for the return value to make sense.
	///
	/// \param[in] pin gpio pin number (kernel representation).
	/// \param[in] value value to write: 0 for low, else high
	///
	/// \return 0 on success
	///			Negative value on failure: -1: reading error (probably pin not enabled). -2: pin enabled but not an output.
	int gpio_digitalWrite(int pin, int value);

#endif
