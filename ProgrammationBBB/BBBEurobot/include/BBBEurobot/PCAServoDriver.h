/// \file PCAServoDriver.h
/// \brief Driver for the servo expander board, based on the PCA9685 chip.
///
/// \details This file implements all the functions to work with the PCA9685, configured as a servo driver.
///	\note	 All functions in this header should be prefixed with servoDriver_.
#ifndef PCASERVO_DRIVER
	#define PCASERVO_DRIVER
	#include "BBBEurobot/I2C-Wrapper.h"

	/// Led driver structure.
	typedef struct {
		I2CAdapter *adapter;		///< I2C port file descriptor.
		guint8 address;				///< Servo driver address.
		int clockFrequency;			///< Device clock frequency in Hz (internal oscillator at 25MHz).
	}ServoDriver;


	/// \brief Initialize the servo driver.
    ///
    /// \details This function tests the communication with the servo driver, and, if successful, inits the structure.
    ///
    /// \param[out] driver The ServoDriver structure, to be used whenever communication with the driver.
    /// \param[in] adapter Pointer to a valid I2CAdapter to choose the I2C port (as returned by the i2c_open function,
    ///                    see I2C-Wrapper.h).
    /// \param[in] address I2C address of the ServoDriver.
    /// \param[in] clockFrequency clock frequency in Hz (either for using external clock, or compensating errors of up
    ///			   to 8% seen on the internal clock - this requires component-specific tuning).
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean servoDriver_init(ServoDriver *driver, I2CAdapter *adapter, guint8 address, int clockFrequency);

	/// \brief Call servoDriver_init with default i2c address (the one implemented in ServoDriver board).
	static inline gboolean servoDriver_initDefault(ServoDriver *driver, I2CAdapter *adapter, int clockFrequency)
	{
		return servoDriver_init(driver, adapter, 0x53, clockFrequency);
	}

	/// \brief Set position of a servo.
    ///
    /// \param[in,out] driver The PCA9635 structure.
    /// \param[in] servo The number of the servo to change (from 0 to 15).
    /// \param[in] position Signal value, in microseconds (clamped between 500 and 2500). Note that the resolution
    ///                   of the driver is of 4us only.
	void servoDriver_setPosition(ServoDriver driver, int servo, int position);
#endif
