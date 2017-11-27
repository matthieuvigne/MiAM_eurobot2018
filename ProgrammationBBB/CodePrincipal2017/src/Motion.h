/// \file Motion.h
/// \brief This file contains all functions related to robot motion on the table.
///
/// \details This file is the only one to have access to the motor controlers. It is responsible
///			 for both moving the robot to specified targets, and updating the robot position.
///	\note	 All functions in this header should be prefixed with motion_.
#ifndef MOTION_H
	#define MOTION_H

	#include "Global.h"
	#include <BBBEurobot/KalmanFilter.h>

	/// \brief Initializes both motors.
	///	\details This function executes all the configuration needed on both motors: SPI bus configuration and
	///			 controler setup.
	///
	/// \return TRUE on success
	gboolean motion_initMotors();

	/// \brief Stops both motors.
	/// \details The motors are then in low impedence mode, i.e. the axle is blocked.
	void motion_stopMotors();
	
	/// \brief Set velocity profile of both motor controller.
	///
    /// \param[in] maxSpeed Maximum motor speed, in steps/s (from 15.25 to 15610, resolution 15.25 step/s).
    /// \param[in] acc Target motor acceleration, in steps/s^2 (from 14.55 to 59590, resolution 14.55 step/s^2).
    /// \param[in] dec Target motor deceleration, in steps/s^2 (from 14.55 to 59590, resolution 14.55 step/s^2).
	void motion_setVelocityProfile(int maxSpeed, int accel, int decel);
	
	/// \brief Calculate rotation angle to go from start to end position.
	///
	/// \param[in] start Robot starting position.
	/// \param[in] end Robot target position.
	/// \returns Angle (in rad) the robot must rotate to go to end.
	double motion_computeRotationAngle(RobotPosition start, RobotPosition end);


	/// \brief Moves the robot a specified distance.
	/// \details Note that this function does a translation of the robot, not of one motor
	///			 (both motors move the same amount). This function blocks until either the
	///			 move is completed, or an obstacle forces the robot to abort motion.
	///			 This motion is done using the controler's position control.
	///
	/// \param[in] distance Distance in mm to move (negative to move backward).
	/// \param[in] readSensor If false, IR sensor value will be ignored during motion. This should be true by default.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_translation(double distance, gboolean readSensor);


	/// \brief Rotate the robot by a certain angle.
	/// \details The angle is a relative angle (in rad) to turn. This function blocks until
	///			 either the move is completed, or an obstacle forces the robot to abort motion.
	///			 This motion is done using speed control of the motors, and a PID controler.
	///
	/// \param[in] angle Angle in rad to turn.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_rotation(double angle);


	/// \brief Rotate the robot to an \b abosule angle.
	/// \details This function simply computes the rotation angle, then calls motion_rotation.
	///
	/// \param[in] angle Angle in rad to turn to.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_rotateToAngle(double angle);


	/// \brief Moves the robot to a desired position.
	/// \details To move the robot, this function first performs a rotation to align
	///			 itself, then a translation to the targeted position. Additionaly, it might
	///			 perform a final rotation to align itself with the position's angle value.
	///			 This function blocks until either the move is completed, or an obstacle forces
	///			 the robot to abort motion.
	///	\note The angle value of pos is not used.
	///
	/// \param[in] pos Target position. Angle value has a meaning only if endRotation argument is TRUE.
	/// \param[in] backward If TRUE, the robot will move backward to the target.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_GoTo(RobotPosition pos, gboolean backward);

	 /// \brief Reset Kalman filter.
	 /// \details If we are sure of the angle (knocking at an edge), we can reset the Kalman Filter
	 /// \note This function is thread-safe.
	void motion_resetKalman(double angle);

	 /// \brief Update the robot position.
	 /// \details This function is called periodically by the GMainLoop (in a timeout).
	 ///		  It uses sensor data and the Kalman filter to update the current position
	 ///		  of the robot.
	 /// \return Always TRUE, to keep the timeout enabled.
	 /// \note This function is thread-safe.
	 gboolean motion_updatePosition();

	 Kalman motionKalman;	///< The kalman filter used by motion_updatePosition()
#endif
