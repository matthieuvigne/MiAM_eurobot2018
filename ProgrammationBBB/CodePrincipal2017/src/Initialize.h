/// \file Initialize.h
/// \brief This file contains all the functions related to robot initialization.
///
/// \details These functions are to be called at robot startup, and are responsible for configuring
///			 and initializing all the components of the robot, so that they are in a suitable
///			 state for the rest of the code to run.
///			 This mainly involves opening the serial ports, checking that the target device is present,
///			 and then waiting for the user to remove the jack.
///	\note	 All functions in this header should be prefixed with init_.

#ifndef INIT_H
	#define INIT_H


	/// \brief The main function for robot startup.
	/// \details This function tries to configure all of the robot elements.
	///			 It should only be called once, at the beginning of the code.
	///			 All failure will result in a message printed, and a change
	///			 in the RGB status led, granted that the led driver is 
	///			 detected.
	void init_MainStartup();



	/// \brief Get the side of the field we are playing on.
	/// \details This function sets the blueSide variable, and updates the led accordingly.
	void init_getSide();
	
	/// \brief This function blocks until the jack has been removed.
	/// \details In addition, it will also try to initialise the motors, 
	/// if they have not yet been initialized (for example because
	/// they were not turned on when the Beaglebone booted).
	void init_waitForStart();
	
	
	/// \brief This function blocks until the jack has been plugged in.
	void init_waitForJackPlug();
#endif
