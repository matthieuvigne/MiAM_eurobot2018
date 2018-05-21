/// \file MotionController.h
/// \brief This file implements function relative to robot motion on the table.
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

	/// \brief Stop both motors, following deceleration curve.
	///
	/// \details This function should be called before code terminates.
	void motion_stopMotors();

	/// \brief Sets the motor driver to high impedence, i.e. release any motor holding torque.
	void motion_releaseMotors();

	/// \brief Performs a hard motor stop, i.e. stop them instantly.
	void motion_stopMotorsHard();

	/// \brief Moves the robot a specified distance.
	/// \details Note that this function does a translation of the robot, not of one motor
	///			 (both motors move the same amount). This function blocks until either the
	///			 move is completed, or an obstacle forces the robot to abort motion.
	///			 This motion is done using the controler's position control.
	///
	/// \param[in] distance Distance in mm to move (negative to move backward).
	/// \param[in] readSensor If false, IR sensor value will be ignored during motion. This should be true by default.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_translate(double distance, gboolean readSensor);

	/// \brief Shake the robot, by doing small translations of opposite sign on the motors.
	///
	/// \param[in] distance Distance in mm to move (negative to move backward).
	gboolean motion_shake(double distance);

	/// \brief Set the velocity profile of the robot.
	///
	/// \param[in] maxSpeed Maximum speed, in steps/s.
	/// \param[in] maxAccel Maximum acceleration, in steps/s{-2}.
	/// \param[in] maxDecel Maximum deceleration, in steps/s{-2}.
	void motion_setVelocityProfile(int maxSpeed, int maxAccel, int maxDecel);

	/// \brief Reset the velocity profile to its default value.
	void motion_resetVelocityProfile();

	/// \brief Rotate the robot of a relative angle.
	/// \details The angle is a relative angle (in rad) to turn. This function blocks until
	///			 the move is completed. This motion is done using speed control of the motors, and a PID controler.
	///
	/// \param[in] angle Angle in rad to turn.
	/// \returns This funciton always return TRUE.
	gboolean motion_rotate(double angle);

	/// \brief Moves the robot to a desired position.
	/// \details To move the robot, this function first performs a rotation to align
	///			 itself, then a translation to the targeted position.
	///			 This function blocks until either the move is completed, or an obstacle forces
	///			 the robot to abort motion.
	///	\note The angle value of pos is not used.
	///
	/// \param[in] pos Target position.
	/// \param[in] backward If TRUE, the robot will move backward to the target.
	/// \param[in] checkInfrared If TRUE, infrared sensors will stop the motion. Otherwise they are ignored.
	/// \returns TRUE if target position has been reached, FALSE if an obstacle has stopped it before.
	gboolean motion_goTo(RobotPosition pos, gboolean backward, gboolean checkInfrared);


	extern gboolean stopOnSwitch;
#endif
