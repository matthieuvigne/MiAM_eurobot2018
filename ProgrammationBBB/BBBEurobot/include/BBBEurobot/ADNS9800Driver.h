/// \file ADNS9800Driver.h
/// \brief Driver for the ADNS9800 mouse sensor.
///
/// \details This file implements all the functions to talk to the stepper motor controller.
/// 		 It is adapted from Sparkfun's driver for Arduino. All functions are thread-safe.
///	\note	 All functions in this header should be prefixed with ANDS9800_.
#ifndef ADNS9800_DRIVER
#define ADNS9800_DRIVER

	#include <stdint.h>
	#include <glib.h>
	
	
	/// L6470 structure.
	typedef struct
	{
		gchar *portName;	///< String representing the name (in the file system) of the SPI port. This must be set by hand before using the structure.
		int port;			///< File descriptor of the SPI port.
		int speed;			///< Communication speed. This must be set by hand before using the structure.
		double resolution;	///< Sensor resolution, in mm/counts.
	}ADNS9800;
	
	/// \brief Initialize the ANDS9800 structure.
	/// \details This function only sets the structure correctly, but does not perform any operation on the 
	///          SPI port. For this reason, it cannot detect a configuration failure, and has no return value.
	///
	/// \param[inout] a An ANDS9800 structure to use to talk to the sensor.
    /// \param[in] portName Name of the port, in the file system (i.e. a string "/dev/spidevx").
    /// \param[in] speed Communication speed.
	void ANDS9800_initStructure(ADNS9800 *a, gchar *portName, int speed);
	
	/// \brief Initialize the ANDS9800 component.
	/// \details This function checks that the sensor is present, configure its registers, and write the firmware.
	///
	/// \param[inout] a An ANDS9800 structure to use to talk to the targeted motor.
    /// \return TRUE on success, FALSE on failure.
	gboolean ADNS9800_performStartup(ADNS9800 *a);
	
	/// \brief Get the motion of the mouse since the last call, in mouse counts.
	/// \param[in] a An ANDS9800 structure to use to talk to the sensor.
    /// \param[out] deltaX Position increment on X axis since last call, in mouse counts.
    /// \param[out] deltaY Position increment on Y axis since last call, in mouse counts.
	void ADNS9800_getMotionCounts(ADNS9800 a, int *deltaX, int *deltaY);
	
	
	/// \brief Get the motion of the mouse since the last call, in mm.
	/// \param[in] a An ANDS9800 structure to use to talk to the sensor.
    /// \param[out] deltaX Position increment on X axis since last call, in mm.
    /// \param[out] deltaY Position increment on Y axis since last call, in mm.
	void ADNS9800_getMotion(ADNS9800 a, double *deltaX, double *deltaY);
#endif
