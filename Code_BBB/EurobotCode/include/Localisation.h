/// \file Localisation.h
/// \brief  This file implements a localisation thread giving the current robot position on the table. This thread
///			also writes sensor and localisation data to an output log file.
///
/// \details The function localisation_start() is meant to be run as a separate thread. This
///			 thread runs a fixed-frequency loop: at each iteration, the position of the robot is estimated.
///	\note	 All functions in this header should be prefixed with localisation_.

#ifndef LOCALISATION_H
	#define LOCALISATION_H

	#include <glib.h>

	/// \brief The localisation thread
	///	\details This start localisation thread. The robot position must have been initialized beforehand.
	///
	/// \note	Doing a thread inside a 'while' might be more accurate than using a timeout, but requires more CPU.
	///			On the beaglebone, errors when using callback or while are both about 8% - however the callback
	///			approch does not take into account time spend in the function. However, the second thread uses about
	///			25% of the CPU, against 3% for the main thread.
	void *localisation_start();

	/// \brief Reset the position of the robot on the table.
	///
	/// \details This function might be used for example when the robot is put in contact with a side of the table,
	///			 to gain back absolute position accuracy.
	///
	/// \param[in] resetPosition The position to which reset the robot.
	/// \param[in] resetX Wheather or not to reset the X coordinate.
	/// \param[in] resetY Wheather or not to reset the Y coordinate.
	/// \param[in] resetTheta Wheather or not to reset the angle.
	void localisation_reset(RobotPosition resetPosition, gboolean resetX, gboolean resetY, gboolean resetTheta);
#endif
