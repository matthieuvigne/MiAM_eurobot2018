/// \file Strategy.h
/// \brief This file contains the strategy of the robot.
///
/// \details What we call strategy is in fact a list of actions to be performed by the robot. This is run in a 
///			 specific thread, called strategieThread.
///	\note	 All functions in this header should be prefixed with strategy_.

#ifndef ST_H
     #define ST_H
    
    /// \brief The strategie thread.
    /// \details This function should be launched in a separate thread at the start of the match. It
    ///			 accomplishes all robot actions, then stops. This thread must be cancelable so that it can be stopped
    ///			 automatically at the end of the timer.
	void *strategy_runMatch();
	
	
    /// \brief Thread containing the funny action.
    /// \details This function will be called 1 second after the end of the match, to execute the funny action.
    ///			 If no funny action is to be executed, this function can just be empty.
	void startegy_funnyAction();
	
	
	/// \brief Reset all servos to their initial position.
	/// \note This function is already called internally, by init_MainStartup for example,
	///		  and does not need to be called additionaly at the start of the match.
	void strategy_servoToInitialPosition();
	
	pthread_t strategyThread;			///< pthread representing the strategy thread (for cancelability)
	 
	 
 #endif 
