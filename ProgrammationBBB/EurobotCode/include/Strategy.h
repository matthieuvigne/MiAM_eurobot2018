/// \file Strategy.h
/// \brief This file contains the strategy of the robot.
///
/// \details What we call strategy is in fact a list of actions to be performed by the robot. This is run in a
///			 specific thread, called strategieThread.
///			 The source file implement all servo-related actions, and motion planning on the field.
///	\note	 All functions in this header should be prefixed with strategy_.

#ifndef ST_H
     #define ST_H

    /// \brief The strategie thread.
    /// \details This function should be launched in a separate thread at the start of the match. It
    ///			 accomplishes all robot actions, then stops. This thread must be cancelable so that it can be stopped
    ///			 automatically at the end of the timer.
	void *strategy_runMatch();

 #endif
