#include "Robot.h"
#include "MotionController.h"

// Max robotMotors speed, in half counts / s.
const int MOTOR_MAX_SPEED = 800;

// Motor current profile constants, for the 42BYGHW810 robotMotors, 2.0A - these values are computed using ST dSPIN utility.
const int MOTOR_KVAL_HOLD = 0x2D;
const int MOTOR_BEMF[4] = {0x3C, 0x172E, 0xE, 0x39};


// Generic implementation of a PID controller.
typedef struct{
	double oldError;	///< Old value of the error, used to compute its derivative.
	double integral; 	///< Integral value.
	double maxIntegral;	///< Maximum value of the integral feedback, i.e Ki * integral (to prevent windup).
	double Kp;			///< Proportional gain.
	double Kd;			///< Derivative gain.
	double Ki;			///< Integral gain.
}PID;


// Compute next value of PID command, given the new error and the elapsed time.
double PID_computeValue(PID *pid, double currentError, double dt)
{
	// Compute velocity.
	double dCurrentError = (currentError - pid->oldError) / dt;
	pid->oldError = currentError;
	// Compute and saturate integral.
	pid->integral += currentError * dt;
	// Prevent division by 0.
	if(ABS(pid->Ki) > 1e-6)
	{
		if(pid->Ki * pid->integral > pid->maxIntegral)
			pid->integral = pid->maxIntegral / pid->Ki;
		if(pid->Ki * pid->integral < -pid->maxIntegral)
			pid->integral = -pid->maxIntegral / pid->Ki;
	}
	// Output PID command.
	return pid->Kp * (currentError + pid->Kd * dCurrentError + pid->Ki * pid->integral);
}

// Helper function: compute rotation angle to go to a specified point.
// The angle returned is in ]-pi,pi]
double computeRotationAngle(RobotPosition start, RobotPosition end)
{
	// Don't rotate if motion is too small.
	if(ABS(end.x - start.x) < 0.002 && ABS(end.y - start.y) < 0.002)
		return 0;
	// Switch y axis sign, because frame is indirect.
	end.y = -end.y;
	start.y = -start.y;

	// Get angle between start and end, between 0 and 2 * PI
	double deltax = end.x - start.x;
	double startEndAngle = 0.0;
	if(deltax==0)
	{
		if(end.y < start.y)
			startEndAngle = 3.0 * G_PI/2.0;
		else
			startEndAngle =  G_PI/2.0;
	}
	else
	{
		if(deltax > 0)
			startEndAngle =  atan( (end.y - start.y)/(end.x - start.x));
		else
			startEndAngle =  G_PI + atan( (end.y - start.y)/(end.x - start.x));
	}

	// Find the rotation angle amount.
	double rotationAngle = start.theta - startEndAngle;
	while(rotationAngle >= G_PI)
		rotationAngle -= 2 * G_PI;
	while(rotationAngle < -G_PI)
		rotationAngle += 2 * G_PI;

	return rotationAngle;
}

// Check infrared sensor, return TRUE if there is an obstacle on the way.
gboolean checkSensor(gboolean backward)
{
	//~ double theta =  modulo2PI(current_getTheta() + M_PI * (backward ? 1 : 0));

	//~ // If we are less that borderDistance from the outside of the field, and
	//~ // looking outside, we disable the sensors to prevent detecting
	//~ // a referee as an obstacle.
	//~ int borderDistance = 500;
	//~ if(theta > M_PI / 2 && theta < 3 * M_PI / 2)
	//~ {
		//~ if( current_getX() < borderDistance)
			//~ return FALSE;
	//~ }
	//~ else
	//~ {
		//~ if(current_getX() > 3000 - borderDistance)
			//~ return FALSE;
	//~ }

	//~ if(theta < M_PI)
	//~ {
		//~ if( current_getY() >2000 - borderDistance)
			//~ return FALSE;
	//~ }
	//~ else
	//~ {
		//~ if(current_getY() < borderDistance)
			//~ return FALSE;
	//~ }
	//~ if(backward)
		//~ return detectionBack;
	//~ return detectionFront;
	return FALSE;
}


gboolean motion_initMotors()
{
	gboolean result = TRUE;

	L6470_initStructure(&robotMotors[RIGHT], SPI_10);
    L6470_initMotion(robotMotors[RIGHT], MOTOR_MAX_SPEED, MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
    L6470_initBEMF(robotMotors[RIGHT], MOTOR_KVAL_HOLD, MOTOR_BEMF[0], MOTOR_BEMF[1], MOTOR_BEMF[2], MOTOR_BEMF[3]);
	if(L6470_getParam(robotMotors[RIGHT], dSPIN_KVAL_HOLD) != MOTOR_KVAL_HOLD)
	{
		printf("Failed to init right robotMotors\n");
		result =  FALSE;
	}
	L6470_initStructure(&robotMotors[LEFT], SPI_11);
    L6470_initMotion(robotMotors[LEFT], MOTOR_MAX_SPEED, MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);
    L6470_initBEMF(robotMotors[LEFT], MOTOR_KVAL_HOLD, MOTOR_BEMF[0], MOTOR_BEMF[1], MOTOR_BEMF[2], MOTOR_BEMF[3]);
	if(L6470_getParam(robotMotors[LEFT], dSPIN_KVAL_HOLD) != MOTOR_KVAL_HOLD)
	{
		printf("Failed to init left robotMotors\n");
		result =  FALSE;
	}

	motion_stopMotors();
	return result;
}


void motion_stopMotors()
{
    L6470_softStop(robotMotors[RIGHT]);
    L6470_softStop(robotMotors[LEFT]);
	L6470_highZ(robotMotors[RIGHT]);
	L6470_highZ(robotMotors[LEFT]);
}


gboolean motion_translate(double distance, gboolean readSensor)
{
	// Don't move if travel distance is less than 2mm.
	if(ABS(distance) < 0.002)
		return TRUE;

	// Clear error register.
	L6470_getError(robotMotors[RIGHT]);
	L6470_getError(robotMotors[LEFT]);

	// Save start encoder value.
	int startEncoder[2];
	for(int i = 0; i < 2; i++)
		startEncoder[i] = L6470_getPosition(robotMotors[i]);

	// Give target to the motors.
	int32_t distancestep = (int) floor(distance / STEP_TO_SI);
	for(int i = 0; i < 2; i++)
		L6470_goToPosition(robotMotors[i], distancestep);

	g_usleep(20000);
	// Wait for motion to be completed.
	while(L6470_isBusy(robotMotors[RIGHT]) != 0 || L6470_isBusy(robotMotors[LEFT]) != 0)
	{
		g_usleep(20000);
		L6470_getError(robotMotors[RIGHT]);
		L6470_getError(robotMotors[LEFT]);

		if(readSensor && checkSensor(distance < 0))
		{
			// An obstacle has been seen: stop the motors, wait 5 sec
			motion_stopMotors();
			g_usleep(5000000);
			// If there is still an obstacle, abort.
			if(checkSensor(distance < 0))
				return FALSE;
			else
			{
				// Try to complete motion.
				L6470_goToPosition(robotMotors[RIGHT], distancestep -  L6470_getPosition(robotMotors[RIGHT]) + startEncoder[RIGHT]);
				L6470_goToPosition(robotMotors[LEFT], distancestep -  L6470_getPosition(robotMotors[LEFT]) + startEncoder[LEFT]);
				startEncoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
				startEncoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);
			}
		}
	}
	motion_stopMotors();
	// Wait for motors to be fully stopped.
	while(L6470_isBusy(robotMotors[RIGHT]) != 0 || L6470_isBusy(robotMotors[LEFT]) != 0)
		g_usleep(20000);
	return TRUE;
}


gboolean motion_rotate(double angle)
{
	// Set control loop period.
	const double LOOP_PERIOD = 0.020;
	double lastLoopTime = 0.0;

	double targetAngle = robot_getPositionTheta() + angle;
	double error = -angle;
	double dError = 0.0;
	double dt = LOOP_PERIOD;

	PID rotationPID = {	error,		// oldError
						0.0,		// integral
						1.0,		// maxIntegral
						120.0,		// Kp
						0.0002,		// Kd
						0.05};		// Ki

	GTimer *motionTimer = g_timer_new();
	g_timer_start(motionTimer);

	// Motion is completed if we are close enough to the target position and our velocity is small enough.
	while(ABS(error) > 0.015 || ABS(dError) > 0.05)
	{
		// Motion is not completed: compute integral value and PID control.
		double command = PID_computeValue(&rotationPID, error, dt);

		// If playing on right side, invert command.
		if(robot_isOnRightSide)
			command = -command;
		// Send motion command to both motors.
		L6470_setSpeed(robotMotors[RIGHT], -command);
		L6470_setSpeed(robotMotors[LEFT], command);

		// Check and clear robotMotors errors.
		L6470_getError(robotMotors[RIGHT]);
		L6470_getError(robotMotors[LEFT]);

		// Wait for time to match loop frequency.
		double currentTime = g_timer_elapsed(motionTimer, NULL);
		while(currentTime - lastLoopTime < LOOP_PERIOD)
		{
			g_usleep(10);
			currentTime = g_timer_elapsed(motionTimer, NULL);
		}
		error = robot_getPositionTheta() - targetAngle;
		dt = currentTime - lastLoopTime;
		dError = (error - rotationPID.oldError) / dt;
		lastLoopTime = currentTime;
	}
	L6470_setSpeed(robotMotors[RIGHT], 0);
	L6470_setSpeed(robotMotors[LEFT], 0);
	// Wait for motors to be fully stopped.
	while(L6470_isBusy(robotMotors[RIGHT]) != 0 || L6470_isBusy(robotMotors[LEFT]) != 0)
		g_usleep(20000);
	return TRUE;
}


gboolean motion_goTo(RobotPosition pos, gboolean backward, gboolean checkInfrared)
{
	// Compute rotation to go to target.
	double rotationAngle = computeRotationAngle(robot_getPosition(), pos);
	if(backward)
	{
		if(rotationAngle < 0)
			rotationAngle += G_PI;
		else
			rotationAngle -=G_PI;
	}
	// Perform the rotation, return if the rotation fails.
	if(!motion_rotate(rotationAngle))
		return FALSE;

	// Calculate the distance.
	double cx = robot_getPositionX();
	double cy = robot_getPositionY();
	double distance = sqrt( (cx - pos.x) * (cx - pos.x) + (cy - pos.y) *(cy - pos.y));
	if(backward)
		distance = -distance;
	// Perform the translation.
	return motion_translate(distance, checkInfrared);
}
