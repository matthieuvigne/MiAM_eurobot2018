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
     /// \details The axes are defined according to the competition rules:
     ///			- the origin is the upper right corner.
     ///			- X is along the long table side, facing right.
     ///			- Y is along the short table side, facing down.
     ///			- Theta is computed as the angle from X to -Y - i.e. the classical trigonometric angle. Note that
     ///			  this corresponds to an INDIRECT frame of reference.
     typedef struct {
		double x;	///< X coordinate of the robot, in mm.
		double y;	///< Y coordinate of the robot, in mm.
		double theta;	///< Angle of the robot, in rad.
	} RobotPosition;

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

	///< Definition of robot hardware.
	IMU robotIMU;
	ADNS9800 robotMouseSensor;
	ColorSensorTCS3472 robotColorSensor;
	LCD robotLCD;
	MaestroDriver robotServo;
	L6470 robotMotors[2];

	///< Robot mechanical dimensions, see Robot.c.
	extern const double STEP_TO_SI; ///< Conversion ratio between a motor step and a millimeter.
	extern const double ROBOT_WIDTH; ///< Distance between both wheels, in mm.
	extern const double CHASSIS_BACK; ///< Distance between wheel axis and back of robot plate.
	extern const double CHASSIS_FRONT; ///< Distance between wheel axis and front of robot plate.
	extern const double CHASSIS_SIDE; ///< Distance between wheel axis and front of robot plate.
	extern const double CANON_OFFSET; ///< Distance between robot center and canon, in mm.
	extern const double CLAW_OFFSET; ///< Distance between robot center and cube claw, in mm.
	extern const double MOUSE_SENSOR_OFFSET; ///< Distance between robot center and mouse sensor, in mm.
	extern const double BALL_LENGTH_OFFSET; ///< Distance between robot center and ball catching part,
	                                        /// along the robot x axis, in mm.
	extern const double BALL_WIDTH_OFFSET; ///< Distance between robot center and ball catching part,
	                                       /// along the robot y axis, in mm.
	extern const double BIN_OFFSET; ///< Distance between robot center and ball throwing bin, in mm.

	extern const double GYRO_Z_BIAS;	///< Initial gyro bias, to remove from gyro signal.
	extern const int RIGHT; ///< Int to represent right motor in array.
	extern const int LEFT; ///< Int to represent left motor in array.

	extern RobotPosition startingPosition; ///< The starting position, defined in MainLoop, but can be used elsewhere.

	///< Boolean to give sensor status about front / back sensor detection.
	gboolean robot_IRDetectionFront, robot_IRDetectionBack;

	///< TRUE if we are playing on the right side of the field, false otherwise.
	gboolean robot_isOnRightSide;


	///< Servo actions. Note that these functions do not wait for the servo action to be done.
	static inline void servo_openWaterTank()
	{
		maestro_setPosition(robotServo, 0, 2125);
	}

	static inline void servo_middleWaterTank()
	{
		//~ maestro_setSpeed(robotServo, 0, 2000);
		maestro_setPosition(robotServo, 0, 1500);
	}

	static inline void servo_closeWaterTank()
	{
		maestro_setPosition(robotServo, 0, 975);
	}

	static inline void servo_ballDirectionCenter()
	{
		maestro_setPosition(robotServo, 1, 1500);
	}

	static inline void servo_ballDirectionBin()
	{
		maestro_setPosition(robotServo, 1, 1800);
	}

	static inline void servo_ballDirectionCanon()
	{
		maestro_setPosition(robotServo, 1, 1200);
	}

	static inline void servo_trayDown()
	{
		maestro_setPosition(robotServo, 2, 1880);
	}

	static inline void servo_trayUp()
	{
		maestro_setPosition(robotServo, 2, 950);
		g_usleep(400000);
		maestro_setPosition(robotServo, 2, 1100);
	}

	// Unlike the other, this function is blocking.
	static inline void servo_openClaws()
	{
		maestro_setPosition(robotServo, 5, 2100);
		while(gpio_digitalRead(CAPE_DIGITAL[1]) == 1)
			g_usleep(10000);
		maestro_setPosition(robotServo, 5, 1500);

	}

	static inline void servo_closeClaws()
	{
		maestro_setPosition(robotServo, 5, 900);
		g_usleep(20000000);
		//~ while(gpio_digitalRead(CAPE_DIGITAL[2]) == 1)
			//~ g_usleep(10000);
		maestro_setPosition(robotServo, 5, 1500);
	}

	static inline void servo_millTurn()
	{
		maestro_setPosition(robotServo, 12, 1375);
	}

	static inline void servo_millTurnBackward()
	{
		maestro_setPosition(robotServo, 12, 1650);
	}

	static inline void servo_millStop()
	{
		maestro_setPosition(robotServo, 12, 0);
	}

	static inline void servo_startCannon()
	{
		maestro_setPosition(robotServo, 13, 1850);
	}

	static inline void servo_stopCannon()
	{
		maestro_setPosition(robotServo, 13, 1500);
	}

	static inline void servo_beeRetract()
	{
		maestro_setPosition(robotServo, 14, 1350);
	}

	static inline void servo_beeLaunch()
	{
		maestro_setPosition(robotServo, 14, 2450);
	}

	static inline void servo_clawUp()
	{
		maestro_setPosition(robotServo, 3, 2480);
		maestro_setPosition(robotServo, 4, 520);
	}

	static inline void servo_clawLightSwitch()
	{
		maestro_setPosition(robotServo, 3, 1870);
		maestro_setPosition(robotServo, 4, 1130);
	}

	static inline void servo_clawDown()
	{
		maestro_setPosition(robotServo, 3, 1720);
		maestro_setPosition(robotServo, 4, 1280);
	}

	/// Send all servos to initial position.
	static inline void servo_initPosition()
	{
		// Turn off servo.
		for(int i = 0; i < 16; i++)
		{
			maestro_setPosition(robotServo, i, 0);
			g_usleep(50000);
		}
		// Remove all speed limits.
		for(int i = 0; i < 16; i++)
		{
			maestro_setSpeed(robotServo, i, 0);
			g_usleep(50000);
		}
		servo_openWaterTank();
		servo_ballDirectionCenter();
		servo_trayDown();
		servo_stopCannon();
		// Bee servo speed
		maestro_setSpeed(robotServo, 14, 700);
		servo_beeRetract();
		maestro_setSpeed(robotServo, 3, 1200);
		maestro_setSpeed(robotServo, 4, 1200);
		servo_clawUp();
	}
 #endif
