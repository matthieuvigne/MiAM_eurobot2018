/// \file PCAServoDriver.h
/// \brief Driver for the servo expander board, based on the PCA9685 chip.
///
/// \details This file implements all the functions to work with the PCA9685, configured as a servo driver.
///	\note	 All functions in this header should be prefixed with servoDriver_.
#ifndef PCASERVO_DRIVER
	#define PCASERVO_DRIVER
	#include <glib.h>
	
	/// Led driver structure.
	typedef struct {
	int port;		///< I2C port file descriptor.
	int address;	///< Servo driver address.
	}ServoDriver;
	
	
	/// \brief Initialize the servo driver.
    ///
    /// \details This function tests the communication with the servo driver, and, if successful, inits the structure.
    ///
    /// \param[out] driver The ServoDriver structure, to be used whenever communication with the driver.
    /// \param[in] port File descriptor of the i2c port (as returned by the i2c_open function, see I2C-Wrapper.h).
    /// \param[in] address I2C address of the ServoDriver.
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean servoDriver_init(ServoDriver *driver, int port, int address);	
	
	/// \brief Set position of a servo.
    ///
    /// \param[in,out] driver The PCA9635 structure (as a pointer as the ledState variable might be modified).
    /// \param[in] servo The number of the servo to change (from 0 to 15).
    /// \param[in] position Signal value, in microseconds (clamped between 500 and 2500). Note that the resolution
    ///                   of the driver is of 4us only.
	void servoDriver_setPosition(ServoDriver driver, int servo, int position);	
#endif
