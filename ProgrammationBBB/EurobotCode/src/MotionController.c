#include "Robot.h"
#include "MotionController.h"

// Max robotMotors speed and acceleration, in half counts / s.
const int MOTOR_MAX_SPEED = 800;
const int MOTOR_MAX_ACCELERATION = 300;

// Motor current profile constants, for the 42BYGHW810 robotMotors, 2.0A - these values are computed using ST dSPIN utility.
const int MOTOR_KVAL_HOLD = 0x2D;
const int MOTOR_BEMF[4] = {0x3C, 0x172E, 0xE, 0x39};


// Generic implementation of a PID controller.
typedef struct{
	double integral; 	///< Integral value.
	double maxIntegral;	///< Maximum value of the integral feedback, i.e Ki * integral (to prevent windup).
	double Kp;			///< Proportional gain.
	double Kd;			///< Derivative gain.
	double Ki;			///< Integral gain.
}PID;


// Compute next value of PID command, given the new error and the elapsed time.
double PID_computeValue(PID *pid, double positionError, double velocityError, double dt)
{
	// Compute and saturate integral.
	pid->integral += positionError * dt;
	// Prevent division by 0.
	if(ABS(pid->Ki) > 1e-6)
	{
		if(pid->Ki * pid->integral > pid->maxIntegral)
			pid->integral = pid->maxIntegral / pid->Ki;
		if(pid->Ki * pid->integral < -pid->maxIntegral)
			pid->integral = -pid->maxIntegral / pid->Ki;
	}
	// Output PID command.
	return pid->Kp * (positionError + pid->Kd * velocityError + pid->Ki * pid->integral);
}



// Trajectory for rotation motion: a velocity trapezoid.
// This function computes trajectory position and velocity at time t.
void rotationTrajectory(double targetAngle, double time, double *position, double *velocity)
{
	// Rotation trajectory velocity parameters.
	double ROTATION_MAX_VELOCITY = 0.9 * MOTOR_MAX_SPEED * STEP_TO_SI / ROBOT_WIDTH * 2.0;
	double ROTATION_MAX_ACCELERATION = 2.0 * MOTOR_MAX_ACCELERATION * STEP_TO_SI / ROBOT_WIDTH * 2.0;
	int sign = 1;
	if(targetAngle < 0)
	{
		sign = -1;
		targetAngle = -targetAngle;
	}

	double rotationDistanceToFullSpeed = ROTATION_MAX_VELOCITY * ROTATION_MAX_VELOCITY / ROTATION_MAX_ACCELERATION / 2.0;
	if(targetAngle < rotationDistanceToFullSpeed)
	{
		// Motion is too short to reach a constant velocity profile: just do an acceleration and deceleration phase.
		double switchTime = sqrt(targetAngle / ROTATION_MAX_ACCELERATION);
		if(time < switchTime)
		{
			*velocity = ROTATION_MAX_ACCELERATION * time;
			*position = ROTATION_MAX_ACCELERATION * time * time / 2.0;
		}
		else
		{

			*velocity = ROTATION_MAX_ACCELERATION * (2 * switchTime - time);
			*position = targetAngle / 2.0 +
			            ROTATION_MAX_ACCELERATION * switchTime * (time - switchTime) -
			            ROTATION_MAX_ACCELERATION * (time - switchTime) * (time - switchTime) / 2.0;
		}
	}
	else
	{
		// We have time for full acceleration.
		double accelerationTime = ROTATION_MAX_VELOCITY / ROTATION_MAX_ACCELERATION;
		double constantVelocityTime = accelerationTime +
		                             (targetAngle - 2 * rotationDistanceToFullSpeed) / ROTATION_MAX_VELOCITY;
		if(time < accelerationTime)
		{
			// Acceleration
			*velocity = ROTATION_MAX_ACCELERATION * time;
			*position = ROTATION_MAX_ACCELERATION * time * time / 2.0;
		}
		else if(time < constantVelocityTime)
		{
			// Constant velocity.
			*velocity = ROTATION_MAX_VELOCITY;
			*position = rotationDistanceToFullSpeed +
			            ROTATION_MAX_VELOCITY * (time - accelerationTime);
		}
		else
		{
			*velocity = ROTATION_MAX_VELOCITY - ROTATION_MAX_ACCELERATION * (time - constantVelocityTime);
			*position = targetAngle - rotationDistanceToFullSpeed +
			            ROTATION_MAX_VELOCITY * (time - constantVelocityTime) -
			            ROTATION_MAX_ACCELERATION * (time - constantVelocityTime) * (time - constantVelocityTime) / 2.0;
		}
	}
	// Clamp trajectory at end.
	if(*position > targetAngle || *velocity < 0.0)
	{
		*position = targetAngle;
		*velocity = 0;
	}
	// Restore sign.
	*position = sign * (*position);
	*velocity = sign * (*velocity);
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
	double rotationAngle = startEndAngle - start.theta;
	while(rotationAngle >= G_PI)
		rotationAngle -= 2 * G_PI;
	while(rotationAngle < -G_PI)
		rotationAngle += 2 * G_PI;

	return rotationAngle;
}

// Distance from the side of field at which we stop listening to IR sensors while moving.
double BORDER_DEADZONE = 500;

// Check infrared sensor, return TRUE if there is an obstacle on the way.
gboolean checkSensor(gboolean backward)
{
	// Create a fictitious point BORDER_DISTANCE in front of the robot. If this point is outside of
	// the field, we ignore IR sensors to prevent detecting a referee as an obstacle.
	RobotPosition robotViewpoint = robot_getPosition();
	// Going backward: add PI to angle to get the back of the robot.
	if(backward)
		robotViewpoint.theta += G_PI;

	robotViewpoint.x += BORDER_DEADZONE * cos(robotViewpoint.theta);
	// Minus sign: the frame is indirect.
	robotViewpoint.y -= BORDER_DEADZONE * sin(robotViewpoint.theta);

	if(robotViewpoint.x < 0 || robotViewpoint.x > 3000 || robotViewpoint.y < 0 || robotViewpoint.y > 2000)
		return FALSE;

	if(backward)
		return robot_IRDetectionBack;
	return robot_IRDetectionFront;
}


gboolean motion_initMotors()
{
	gboolean result = TRUE;

	L6470_initStructure(&robotMotors[RIGHT], SPI_10);
    L6470_initMotion(robotMotors[RIGHT], MOTOR_MAX_SPEED, MOTOR_MAX_ACCELERATION);
    L6470_initBEMF(robotMotors[RIGHT], MOTOR_KVAL_HOLD, MOTOR_BEMF[0], MOTOR_BEMF[1], MOTOR_BEMF[2], MOTOR_BEMF[3]);
	if(L6470_getParam(robotMotors[RIGHT], dSPIN_KVAL_HOLD) != MOTOR_KVAL_HOLD)
	{
		printf("Failed to init right robotMotors\n");
		result =  FALSE;
	}
	L6470_initStructure(&robotMotors[LEFT], SPI_11);
    L6470_initMotion(robotMotors[LEFT], MOTOR_MAX_SPEED, MOTOR_MAX_ACCELERATION);
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


gboolean motion_rotate(double motionAngle)
{
	// Set control loop period.
	const double LOOP_PERIOD = 0.020;

	double startAngle = robot_getPositionTheta();
	double targetAngle = startAngle + motionAngle;

	double currentTime = 0.0;
	double dt = LOOP_PERIOD;

	double currentPosition = startAngle;
	double currentVelocity = 0.0;
	double lastPosition = currentPosition;
	double lastTime = 0.0;


	PID rotationPID = {0.0,		// integral
	                   1.0,		// maxIntegral
	                   120.0,		// Kp
	                   0.0002,		// Kd
	                   0.05};		// Ki

	GTimer *motionTimer = g_timer_new();
	g_timer_start(motionTimer);

	// Motion is completed if we are close enough to the target position and our velocity is small enough.
	while((ABS(currentPosition - targetAngle) > 0.015 || ABS(currentVelocity) > 0.05) && currentTime < 8)
	{
		// Get trajectory value.
		double trajectoryPosition, trajectoryVelocity;
		rotationTrajectory(motionAngle, currentTime, &trajectoryPosition, &trajectoryVelocity);
		trajectoryPosition += startAngle;
		// Compute PID command
		double command = PID_computeValue(&rotationPID,
		                                  currentPosition - trajectoryPosition,
		                                  currentVelocity - trajectoryVelocity,
		                                  dt);
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
		currentTime = g_timer_elapsed(motionTimer, NULL);
		while(currentTime - lastTime < LOOP_PERIOD)
		{
			g_usleep(10);
			currentTime = g_timer_elapsed(motionTimer, NULL);
		}
		// Compute current position, error, velocity.
		currentPosition = robot_getPositionTheta();
		dt = (currentTime - lastTime);
		currentVelocity = (currentPosition - lastPosition) / dt;
		lastPosition = currentPosition;
		lastTime = currentTime;
	}
	if(currentTime >= 8)
		printf("ROTATION MOTION TIMEOUT\n");
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
