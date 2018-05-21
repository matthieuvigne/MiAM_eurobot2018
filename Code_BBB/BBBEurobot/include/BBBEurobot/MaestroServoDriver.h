/// \file MaestroServoDriver.h
/// \brief Driver for the maestro servo drivers, using UART.
///
/// \details For now only target setting via pololu protocol is supported.
///	\note	 All functions in this header should be prefixed with maestro_.
#ifndef MAESTROSERVO_DRIVER
	#define MAESTROSERVO_DRIVER

	#include <glib.h>

	/// MaestroDriver structure: a simple int representing the board communication port.
	typedef struct{
		int port;		///< Serial port file descriptor.
		int deviceID; 	///< Pololu device ID, for daisy chaining.
	}MaestroDriver;


	/// \brief Initialize the servo driver.
    ///
    /// \param driver MaestroDriver structure to initialize.
    /// \param portName Serial port file name ("/dev/ttyOx")
    /// \param deviceID Maestro device ID.
    /// \returns   TRUE on success, FALSE otherwise.
	gboolean maestro_init(MaestroDriver *driver, gchar *portName, int deviceID);

	static inline gboolean maestro_initDefault(MaestroDriver *driver, gchar *portName)
	{
		return maestro_init(driver, portName, 12);
	}

	/// \brief Set target position of a servo.
    ///
    /// \param[in,out] driver The MaestroDriver structure.
    /// \param[in] servo The number of the servo to change (from 0 to 15).
    /// \param[in] position Signal value, in microseconds (clamped between 500 and 2500). Note that the resolution
    ///                   of the driver is of 0.25 microseconds.
	void maestro_setPosition(MaestroDriver driver, int servo, double position);

	/// \brief Set target speed of a servo.
	/// \details This function in itself does not move a servo, but only specify the speed at which it will move at.
    ///
    /// \param[in,out] driver The MaestroDriver structure.
    /// \param[in] servo The number of the servo to change (from 0 to 15).
    /// \param[in] speed Servo speed, in us/s. Device resolution is 25us/s
	void maestro_setSpeed(MaestroDriver driver, int servo, int speed);
#endif
