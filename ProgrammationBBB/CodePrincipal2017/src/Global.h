/// \file Global.h
/// \brief This file contains global variables, as well as standard includes that are needed by all the files in src.
///
/// \details This header is included in all the files in the src folder (but not the driver folder, that must remain
///			 independant). In this file are defined variables that need to be accessed accross several files, the
///			 headers needed to define them, as well as stdlib, math, stdio and glib, which are needed by almost all
///			 the files in src. It also contains definitions for the getters and setters of the current position 
/// 		 (thread-safe).
#ifndef GLOB_H
     #define GLOB_H
     
	///< Global includes
	#include <BBBEurobot/IMUDriver.h>
	#include <BBBEurobot/PCA9635Driver.h>
	#include <BBBEurobot/L6470Driver.h>
	#include <BBBEurobot/Ax12Driver.h>
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
		double theta;	///< Angle of the robot.
	} RobotPosition;
	
	
	/// \note Access to the current position must be thread-safe, as several threads might access them at the same time.
	/// 	  For this reason, the RobotPosition variable current is only available through the following, thread-safe
	///		  functions, that are defined in Current.c.
	double current_getX();	///< Get current x position.
	double current_getY();	///< Get current y position.
	double current_getTheta();	///< Get current angle.
	
	void current_setX(double x);	///< Set current x position.
	void current_setY(double y);	///< Set current y position.
	void current_setTheta(double t); ///< Set current angle.
	
	void current_incX(double x);	///< Increment current x position.
	void current_incY(double y);	///< Increment current y position.
	
	void current_set(RobotPosition pos);	///< Set full robot position.
	RobotPosition current_get();			///< Get full robot position.
	
	
	IMU imu;	///< The robot IMU.
	PCA9635 leddriver;	///< The robot led driver.
	Ax12 ax12;		///< The robot Ax12 communication port.
	
	
	gboolean detectionFront, detectionBack;	///< Robot detection variables.
	
	///< Match timer, this is also used by the Kalman filter to know the time elapsed since the last call.
	GTimer *start;
	
	///< TRUE if we are playing on the blue side of the field.
	gboolean blueSide;	 
 #endif 
