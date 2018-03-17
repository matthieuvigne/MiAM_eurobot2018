/// \file Robot.h
/// \brief This file defines global variables representing the hardware on the robot, that needs sharing between
///        different files.
///
/// \details This header is included in all other source files. It contains variables representing each element of the
///			 robot, as well as global variable. The current position of the robot, and its target, are made available
///			 through mutex-proetected variables. For all other elements of this file, it is to the user to ensure
///			 no race condition may occur. Note that the I2C port is implemented in a thread-safe way, so as long
///			 as the object themselves are not modified, there should be no issue.
#ifndef ROBOT_H
     #define ROBOT_H

	///< Global includes
	#include <BBBEurobot/BBBEurobot.h>
	#include <math.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <glib.h>

     /// \brief Structure used to represent the position of the robot
     /// \details The origin is taken at the lower left corner, X axis aligned along the longest
     ///		  side of the table, theta is the angle between the vector going from back to front of the robot
     ///		  and the x unit vector, positive in the trigonometric direction.
     typedef struct {
		double x;	///< X coordinate of the robot.
		double y;	///< Y coordinate of the robot.
		double theta;	///< Angle of the robot, in rad.
	} RobotPosition;

	///< Type of target that may be sent to the robot
	typedef enum{
		STOP = 0,
		TRANSLATION = 1,
		ROTATION = 2
	}TargetType_t;

	/// \brief Structure to pass a target to the motion controller
	/// \details This structure contains two fields: type specifies the type of target,
	///			 parameter is a robot target position - depending on  the motion type only part of this parameter
	///          is parsed.
	typedef struct{
		TargetType_t type;
		RobotPosition targetPosition;
	}RobotTarget;


	/// \note Access to the current position must be thread-safe, as several threads might access them at the same time.
	/// 	  For this reason, the RobotPosition variable current is only available through the following, thread-safe
	///		  functions, that are defined in Robot.c.
	double robot_getPositionX();	///< Get position x position.
	double robot_getPositionY();	///< Get position y position.
	double robot_getPositionTheta();	///< Get position angle.

	void robot_setPositionX(double x);	///< Set position x position.
	void robot_setPositionY(double y);	///< Set position y position.
	void robot_setPositionTheta(double t); ///< Set position angle.

	void robot_setPosition(RobotPosition pos);	///< Set full robot position.
	RobotPosition robot_getPosition();			///< Get full robot position.

	/// \note Like for the current position, access to the current target is thread-safe.
	RobotTarget robot_getTarget();
	void robot_setTarget(RobotTarget target);


	///< Definition of robot hardware.
	IMU robotIMU;
	ADNS9800 robotMouseSensor;
	ColorSensorTCS3472 robotColorSensor;
	LCD robotLCD;
	ServoDriver robotServo;
	L6470 robotMotors[2];

	///< Shared variables - to be updated...
	gboolean detectionFront, detectionBack;	///< Robot detection variables.

	///< TRUE if we are playing on the blue side of the field.
	gboolean blueSide;
 #endif
