/// \file Config.h
/// \brief This file contains all the constants relative to a specific robot configuration.
///
/// \details This file is a header-only definition of constants that caracterise the current robot. The objective is to
///			 gather in one file everything that might be robot dependant: thus, idealy, changes in this file are enough
///			 to switch from principal to secondary robot. Thus, everything relative to sensor/motor placement is
///			 defined here.
///			 Definition are organised as follow:
///			  - All the variables beginning with CAPE_ are defined in hardware by the Beaglebone cape PCB. As this PCB
///				is the same between both robots, it should not change between both files. Any change in these variables
///				can only happen if there is a change in the PCB wiring.
///			  - All the variables prefixed with ROOF_ depend on the PCB that is fixed on the roof of the robot.
///				Theoretically it should also be identical, however LED color is different between both PCBs, thus requiering
///				some adjustments
///			  - All the variables prefixed with DIMENSIONS_ depend on the physical layout of the robot, i.e. its size.
///			  - All the variables prefixed with AX12_ depend on the servomotors that were choosen. 
#ifndef CONFIG_H
	#define CONFIG_H

	#include <BBBEurobot/Ax12Driver.h>

	static gchar* const ROBOT_NAME = "Primary Robot";	///< Name of the robot, can be used to identify the running code.

	// All CAPE_ variables, dependant only on the Beaglebone cape
	static gchar* const CAPE_I2C = "/dev/i2c-1";	///< Name of the I2C port used on the robot.

	static gchar* const CAPE_AX12_PORT = "/dev/ttyO1"; ///< Name of the UART port used for the Ax12.
	static int const CAPE_AX12_DIRECTION_PIN = 46; ///< GPIO number of the Ax12 direction pin.

	static gchar* const CAPE_RIGHT_MOTOR_PORT = "/dev/spidev1.1";  ///< Name of the SPI port used for the right motor.
	static gchar* const CAPE_LEFT_MOTOR_PORT = "/dev/spidev1.0";  ///< Name of the SPI port used for the left motor.

	/// \brief Mapping between the Beaglebone analogic input and the cape.
	/// \details Three analogic input are exposed on the cape. Starting from the bottom of the PCB
	///		  (i.e. the side closest to the power connector), this array gives the mapping
	///		  to the corresponding Beaglebone inputs.
	static int const CAPE_ANALOG[3] = {6,5,4};


	// All ROOF_ variables, dependant only on the roof PCB
	static int const ROOF_SIDE_CHOOSING_SWITCH = 65;	///< GPIO number of the switch used to choose the team of the robot.
	static int const ROOF_STARTING_SWITCH = 22;	///< GPIO number of the switch used to start the robot.

	/// \brief The number of the RGB status LED.
	/// \details This LED is used to get information on the state of the robot, according to the following color code:
	///		   - GREEN : all OK
	///		   - RED : couldn't init IMU
	///		   - PURPLE : couldn't talk to motors
	///		   - BLUE : couldn't talk to Ax12
	///		   - ORANGE : couldn't talk to IR beacon
	static int const ROOF_RGB_STATUS = 1;
	static int const ROOF_RGB_UNASSIGNED = 2; ///< Number of third LED on the PCB, so far with no specific function.
	static int const ROOF_RGB_TEAM = 3; ///< Number of the LED used to tell which side the robot is on.

	static int const ROOF_HEARTBEAT = 15;	///< Pin number of the heartbeat LED
	static int const ROOF_OBSTACLE_FRONT = 11; ///< Pin number of LED to indicate that an obstacle is seen in front of the robot.
	static int const ROOF_OBSTACLE_BACK = 8; ///< Pin number of LED to indicate that an obstacle is seen behind the robot.
	static int const ROOF_DEBUG_ERR = 9; ///< Pin number of red debug LED
	static int const ROOF_DEBUG_INFO = 10; ///< Pin number of blue debug LED

	// The following variables depend on the robot dimensions.
	/// \brief Distance (in mm) between the robot position and the very back of the robot.
	/// \details Remember that the robot position is in fact the position of the midpoint between the wheels.
	/// \note A macro define is used here as this variable is used latter to initialize the STARTING_POSITION,
	///		  thus needs to be a compile time constant (which is not the case of a const).
	#define DIMENSIONS_BACK_OFFSET 55	
	static double const DIMENSIONS_FRONT_OFFSET = 89;	///< Distance (in mm) between the robot position and the front of the robot.	

	/// \brief Distance (in mm) between both wheels.
	/// \details This distance is used to get a rotation angle estimate from the encoders.
	///			 It might not correspond exactly to the distance between the physical wheels, as there is not in fact
	///			 a ponctual contact between the wheels and the ground.	
	static double const DIMENSIONS_DISTANCE_BETWEEN_WHEELS = 280.0;	

	static double const DIMENSIONS_STEP_TO_MM = 1.422;	///< Distance in mm corresponding to one motor step.

	// The following variables depend on the IR reciever wiring
	static int const IR_FRONT[2] = {0,1};	///< Position of the two front IR sensor. -1 means sensor is not used.
	static int const IR_BACK = 2;			///< Position of the back IR sensor. -1 means sensor is not used.
	
	static int const IR_FRONT_DETECTION = 550; ///< Value of the IR sensor above which an obstacle is considered to be seen at the front of the robot.
	static int const IR_BACK_DETECTION = 550; ///< Value of the IR sensor above which an obstacle is considered to be seen at the back of the robot.
	
	// The following variables are to be set by the programmer.
	/// \brief Average gyro offset (in deg/s), on x, y and z channel.
	/// \details This is \b substracted from the gyro reading to get an unbias (null-centered) value (before correcting the rotation).
	static double const GYRO_OFFSET[3] = {8.648, -0.433, -0.540};
	
	/// \brief Gyro rotation.
	/// \details This rotation is calculated in order to realign the acceleration vector with the gravity (i.e align the sensor with the world).
	///			 Because of mounting errors indeed, the sensor might not be exactly aligned with the vertical.
	///			 This should be a rotation matrix, but because we are only interested in the z value of the gyro,
	///			 only the last line is saved.
	static double const GYRO_ROTATION[3] = {0,0,-1};

	static int const MOTOR_MAX_SPEED = 200;	///< Maximum speed of the stepper motor, in steps/s. This is the speed that will be used when doing a translation.

	static double const PID_ROT_P = 110.0;	///< Proportional gain of the PID controler servoing the rotation.
	static double const PID_ROT_I = 0.0054;	///< Integral gain of the PID controler servoing the rotation.
	static double const PID_ROT_D = 0.0059;	///< Derivative gain of the PID controler servoing the rotation.

	/// \brief Initial position of the robot.
	/// \details The initial position is defined relative to the left side of the field:
	///			 for a symmetrical robot, the starting position on the other side will remain
	///			 the same, but the way the angle is calculated in flipped (see Motion.c).
	static RobotPosition const STARTING_POSITION = {1070-135, 2000 - DIMENSIONS_BACK_OFFSET, -M_PI_2};

	/// \brief motor current consumption and values for BEMF compensation, as computed by ST dSPIN utility.
	/// \note Values for the 42BYGHW811 motor.
	#define MOTOR_KVAL_HOLD 0x28
	#define MOTOR_BEMF 0x37, 0x1CF9, 0x34, 0x54
	
	// The following variables depends on the Ax12 seros used
	static int const AX12_FUNNY_ACTION = 11;	///< Servo used to control the funny action.
	static int const AX12_CYLINDER_FALL = 12;	///< Servo used to make the cylinders fall.
	static int const AX12_CYLINDER_GRAB = 9;	///< Servo used to grab the cylinders.
	static int const AX12_WHEEL = 8;	///< Servo used to grab the cylinders.


	static inline void AX12_extendCylinderFinger(Ax12 ax12) 
	{
		ax12_setPosition(ax12, AX12_CYLINDER_FALL, 440);
	}


	static inline void AX12_retractCylinderFinger(Ax12 ax12) 
	{
		ax12_setPosition(ax12, AX12_CYLINDER_FALL, 725); 
	}


	static inline void AX12_launchRocket(Ax12 ax12) 
	{ 
		ax12_setPosition(ax12, AX12_FUNNY_ACTION, 285); 
	}

	static inline void AX12_grabRocket(Ax12 ax12) 
	{ 
		ax12_setPosition(ax12, AX12_FUNNY_ACTION, 210); 
	}
	
	static inline void AX12_grabCylinder(Ax12 ax12) 
	{ 
		ax12_setPosition(ax12, AX12_CYLINDER_GRAB, 300); 
	}

	static inline void AX12_releaseCylinder(Ax12 ax12) 
	{ 
		ax12_setPosition(ax12, AX12_CYLINDER_GRAB, 0); 
	}
	
	static inline void AX12_retractCylinder(Ax12 ax12) 
	{ 
		ax12_setPosition(ax12, AX12_CYLINDER_GRAB, 530); 
	}
	
	static inline void AX12_spinWheel(Ax12 ax12) 
	{ 
		ax12_setSpeed(ax12, AX12_WHEEL, 1000); 
	}
	
	static inline void AX12_reverseWheel(Ax12 ax12) 
	{ 
		ax12_setSpeed(ax12, AX12_WHEEL, 2000); 
	}
	
	static inline void AX12_stopWheel(Ax12 ax12) 
	{ 
		ax12_setSpeed(ax12, AX12_WHEEL, 0); 
	}
#endif 
