/// \file MotionController.h
/// \brief This file implements all task related to positionning the robot on the table.
///
/// \details The function motion_startController() is meant to be run as a separate thread. This
///			 thread runs a fixed-frequency loop: at each iteration, the position of the robot is estimated,
///			 and the robot is moved towards the current target, if needed.
///	\note	 All functions in this header should be prefixed with motion_.

#ifndef MOTION_H
	#define MOTION_H

	#include <glib.h>

	/// \brief Initializes both motors.
	///	\details This function executes all the configuration needed on both motors: SPI bus configuration and
	///			 controler setup.
	///
	/// \return TRUE on success, FALSE on failure. Failure may simply be due to the controller being turned off,
	///			and should be retested.
	gboolean motion_initMotors();

	/// \brief The motion controller thread
	///	\details This start motion controller - note that, on startup, the controller sets the target to STOP
	///			 but does not set the robot initial position.
	///
	/// \note	Doing a thread inside a 'while' might be more accurate than using a timeout, but requires more CPU.
	///			On the beaglebone, errors when using callback or while are both about 8% - however the callback
	///			approch does not take into account time spend in the function. However, the second thread uses about
	///			25% of the CPU, against 3% for the main thread.
	void *motion_startController();

	/// \brief Stop the motion controller.
	///
	/// \details This function sends a stop signal to the motion controller, and wait a full motionController iteration.
	///			 The motion controller is stopped on the next loop check, and thus should be stopped before exiting this
	///			 function.
	void motion_stopController();
#endif
