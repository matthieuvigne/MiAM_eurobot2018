#include "Robot.h"
#include "MotionController.h"

// Update loop frequency
const double LOOP_PERIOD = 0.010;
// Boolean to turn off the controller.
gboolean isMotionControllerRunning;

// Robot-specific constants.

// Max motor speed, in half counts / s.
const int MOTOR_MAX_SPEED = 200;

// Motor current profile constants, for the 42BYGHW810 motor, 1.8A - these values are computed using ST dSPIN utility.
const int MOTOR_KVAL_HOLD = 0x26;
const int MOTOR_BEMF[4] = {0x2D, 0x172E, 0xE, 0x2E};


// Constants for choosing side in arrays.
#define RIGHT 0
#define LEFT 1

//~ int32_t encoder[2];	// Encoder data.
//~ int32_t oldEncoder[2]; // Previous encoder data.

//~ GMutex motionKalmanMutex;

//~ // For testing: log angle data to log file
//~ GIOChannel *logFile = NULL;
//~ double logTime = 0;
//~ double logAngleGyro = 0;	// Angle obtained through pure gyro integration
//~ double logAngleEncoder = 0;	// Angle obtained through pure encoder integration


//~ // Absolute value for double
//~ double absdouble (double a)
//~ {
    //~ if (a < 0 ) return -a;
    //~ return a;
//~ }


//~ // Return an angle between 0 and 2 * PI
//~ double modulo2PI(double x)
//~ {
    //~ while(x > 2 * M_PI)
		//~ x -= 2*M_PI;
    //~ while(x < 0)
		//~ x += 2*M_PI;
    //~ return x;
//~ }


// Check infrared sensor, return TRUE if there is an obstacle on the way.
//~ gboolean checkSensor(gboolean backward)
//~ {
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

//~ }

// Stop both motors.
void stopMotors()
{
    L6470_softStop(robotMotors[RIGHT]);
    L6470_softStop(robotMotors[LEFT]);
	L6470_highZ(robotMotors[RIGHT]);
	L6470_highZ(robotMotors[LEFT]);
}

// Callaback function called when this thread is killed.
void killMotion()
{
	printf("Killing motion thread\n");
	stopMotors();
	exit(0);
}

gboolean motion_initMotors()
{
	gboolean result = TRUE;

	L6470_initStructure(&robotMotors[RIGHT], SPI_10);
    L6470_initMotion(robotMotors[RIGHT], MOTOR_MAX_SPEED, 0.5*MOTOR_MAX_SPEED, 0.5*MOTOR_MAX_SPEED);
    L6470_initBEMF(robotMotors[RIGHT], MOTOR_KVAL_HOLD, MOTOR_BEMF[0], MOTOR_BEMF[1], MOTOR_BEMF[2], MOTOR_BEMF[3]);
	if(L6470_getParam(robotMotors[RIGHT], dSPIN_KVAL_HOLD) != MOTOR_KVAL_HOLD)
	{
		printf("Failed to init right motor\n");
		result =  FALSE;
	}
	L6470_highZ(robotMotors[RIGHT]);

	printf("%d\n", L6470_getParam(robotMotors[RIGHT], dSPIN_STEP_MODE));
	printf("%d\n", L6470_getError(robotMotors[RIGHT]));
	printf("%d\n", L6470_getError(robotMotors[RIGHT]));

	L6470_initStructure(&robotMotors[LEFT], SPI_11);
    L6470_initMotion(robotMotors[LEFT], MOTOR_MAX_SPEED, 0.5*MOTOR_MAX_SPEED, 0.5*MOTOR_MAX_SPEED);
    L6470_initBEMF(robotMotors[LEFT], MOTOR_KVAL_HOLD, MOTOR_BEMF[0], MOTOR_BEMF[1], MOTOR_BEMF[2], MOTOR_BEMF[3]);
	if(L6470_getParam(robotMotors[LEFT], dSPIN_KVAL_HOLD) != MOTOR_KVAL_HOLD)
	{
		printf("Failed to init left motor\n");
		result =  FALSE;
	}

	//~ stopMotors();
	return result;
}


//~ void motion_setVelocityProfile(int maxSpeed, int accel, int decel)
//~ {
	//~ L6470_setVelocityProfile(robotMotors[RIGHT], maxSpeed, accel, decel);
	//~ L6470_setVelocityProfile(robotMotors[LEFT], maxSpeed, accel, decel);
//~ }

//~ double motion_computeRotationAngle(RobotPosition start, RobotPosition end)
//~ {
    //~ if( absdouble (end.x - start.x) < 2 && absdouble (end.y - start.y) < 2)
        //~ return 0;
    //~ double deltax = end.x - start.x;
    //~ double alpha;
    //~ if(deltax==0)
    //~ {
        //~ if(end.y < start.y)
            //~ alpha = 3.0 * M_PI/2.0;
        //~ else
            //~ alpha =  M_PI/2.0;
    //~ }
    //~ else if(deltax > 0)
        //~ alpha =  atan( (end.y - start.y)/(end.x - start.x));
    //~ else
        //~ alpha =  M_PI + atan( (end.y - start.y)/(end.x - start.x));

    //~ double angle = modulo2PI(alpha - start.theta);
    //~ if(angle > M_PI)
        //~ angle -= 2.0 * M_PI;


    //~ return angle;
//~ }


//~ gboolean motion_translation(double distance, gboolean readSensor)
//~ {
	//~ if(absdouble(distance) < 2)
		//~ return TRUE;

	//~ // Read error register, and display if there is an error.
	//~ int controllerError[2];
	//~ controllerError[RIGHT] = L6470_getError(robotMotors[RIGHT]);
	//~ controllerError[LEFT] = L6470_getError(robotMotors[LEFT]);
	//~ if(controllerError[LEFT] != 0)
		//~ printf("Left motor error: %d\n", controllerError[LEFT]);
	//~ if(controllerError[RIGHT] != 0)
		//~ printf("Right motor error: %d\n", controllerError[RIGHT]);

	//~ // Save start encoder value.
	//~ int startEncoder[2];
	//~ startEncoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
	//~ startEncoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);

	//~ // Give target to the motors.
	//~ int32_t distancestep = (int) floor(distance / DIMENSIONS_STEP_TO_MM);
	//~ L6470_goToPosition(robotMotors[LEFT], distancestep);
	//~ L6470_goToPosition(robotMotors[RIGHT], distancestep);
	//~ g_usleep(20000);
	//~ // Wait for motion to be completed.
	//~ while(L6470_isBusy(robotMotors[RIGHT]) != 0 || L6470_isBusy(robotMotors[LEFT]) != 0)
	//~ {
		//~ g_usleep(20000);
		//~ controllerError[RIGHT] = L6470_getError(robotMotors[RIGHT]);
		//~ controllerError[LEFT] = L6470_getError(robotMotors[LEFT]);
		//~ if(controllerError[LEFT] != 0)
			//~ printf("Left motor error: %d\n", controllerError[LEFT]);
		//~ if(controllerError[RIGHT] != 0)
			//~ printf("Right motor error: %d\n", controllerError[RIGHT]);
		//~ if(readSensor && checkSensor(distance < 0))
		//~ {
			//~ // An obstacle has been seen: stop the motors, wait 5 sec
			//~ motion_stopMotors();
			//~ g_usleep(5000000);
			//~ // If there is still an obstacle, abort.
			//~ if(checkSensor(distance < 0))
				//~ return FALSE;
			//~ else
			//~ {
				//~ // Try to complete motion.
				//~ L6470_goToPosition(robotMotors[RIGHT], distancestep -  L6470_getPosition(robotMotors[RIGHT]) + startEncoder[RIGHT]);
				//~ L6470_goToPosition(robotMotors[LEFT], distancestep -  L6470_getPosition(robotMotors[LEFT]) + startEncoder[LEFT]);
				//~ startEncoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
				//~ startEncoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);
			//~ }
		//~ }
	//~ }
	//~ motion_stopMotors();
	//~ g_usleep(150000);
	//~ return TRUE;
//~ }


//~ gboolean motion_rotation(double angle)
//~ {
	//~ if(absdouble(angle) < 0.05)
		//~ return TRUE;

	//~ double currentAngle = current_getTheta();
	//~ double target = currentAngle + angle;

	//~ int controllerError[2];

	//~ double integral = 0;
	//~ int command = 0;
	//~ double olderror = 0;
	//~ double error = currentAngle - target;
	//~ // Stop only once we have reached the target, and the speed is almost 0
	//~ while( absdouble(currentAngle - target) > 0.01 || absdouble(olderror - error) > 0.01)
	//~ {
		//~ currentAngle = current_getTheta();
		//~ olderror = error;
		//~ error = currentAngle - target;

		//~ // Do not compute integral if we are too far from the target.
		//~ if(absdouble(error) < 0.5)
			//~ integral += error;
		//~ command = (int) (PID_ROT_P * (error + PID_ROT_D * (error - olderror ) + PID_ROT_I * integral));
		//~ command *= (blueSide ? 1 : -1);

		//~ // Clamp command to max speed.
		//~ if(command > MOTOR_MAX_SPEED)
			//~ command = MOTOR_MAX_SPEED;
		//~ if(command < -MOTOR_MAX_SPEED)
			//~ command = -MOTOR_MAX_SPEED;

		//~ // Check for an error that might prevent the motor from starting, and display it.
		//~ controllerError[RIGHT] = L6470_getError(robotMotors[RIGHT]);
		//~ controllerError[LEFT] = L6470_getError(robotMotors[LEFT]);
		//~ if(controllerError[LEFT] != 0)
			//~ printf("Left motor error: %d\n", controllerError[LEFT]);
		//~ if(controllerError[RIGHT] != 0)
			//~ printf("Right motor error: %d\n", controllerError[RIGHT]);

		//~ L6470_setSpeed(robotMotors[RIGHT], -command);
		//~ L6470_setSpeed(robotMotors[LEFT], command);
		//~ g_usleep(20000);
		//~ // Don't check the infrared sensors when doing a rotation.
	//~ }
	//~ motion_stopMotors();
	//~ g_usleep(150000);
	//~ return TRUE;
//~ }


//~ gboolean motion_rotateToAngle(double absoluteAngle)
//~ {
	//~ double angle = modulo2PI(absoluteAngle) - modulo2PI(current_getTheta());
	//~ if(angle > M_PI)
		//~ angle -=2 * M_PI;
	//~ if(angle < -M_PI)
		//~ angle += 2 * M_PI;
	//~ return motion_rotation(angle);
//~ }


//~ gboolean motion_GoTo(RobotPosition pos, gboolean backward)
//~ {
	//~ // Calculate the angle to rotate.
	//~ double angle = motion_computeRotationAngle(current_get(), pos);
	//~ if(backward)
	//~ {
		//~ if(angle < 0)
			//~ angle += M_PI;
		//~ else
			//~ angle -=M_PI;
	//~ }
	//~ // Perform the rotation, return if the rotation fails.
	//~ if(!motion_rotation(angle))
		//~ return FALSE;
	//~ // Calculate the distance.
	//~ double cx = current_getX();
	//~ double cy = current_getY();
	//~ double distance = sqrt( (cx - pos.x) * (cx - pos.x) + (cy - pos.y) *(cy - pos.y));
	//~ if(backward)
		//~ distance = -distance;
	//~ // Perform the translation, return if it fails.
	//~ if(!motion_translation(distance, TRUE))
		//~ return FALSE;

	//~ return TRUE;
//~ }

//~ void motion_resetKalman(double angle)
//~ {
	//~ g_mutex_lock(&motionKalmanMutex);
	//~ current_setTheta(angle);
	//~ kalman_init(&motionKalman, angle);
	//~ g_mutex_unlock(&motionKalmanMutex);
//~ }


//~ // Returns the corrected reading of the gyro along the z axis.
//~ // First, sensor offset is removed from all channels. Then, a rotation is applied to realign the sensor with
//~ // the vertical in the world frame.
//~ // Returns the angular speed around the (world) vertical axis, in rad/s).
//~ double getCorrectedGyroZReading()
//~ {
	//~ // Get reading along all axis.
	//~ double gyroReadings[3];
	//~ imu_gyroGetValues(imu, &gyroReadings[0],&gyroReadings[1],&gyroReadings[2]);
	//~ // For each axis, remove offset and multiply by rotation matrix.
	//~ double correctedZValue = 0;
	//~ for(int i = 0; i < 3; i++)
		//~ correctedZValue+= GYRO_ROTATION[i] * (gyroReadings[i] - GYRO_OFFSET[i]);
	//~ // Convert from deg to rad.
	//~ correctedZValue *= M_PI / 180.0;
	//~ return correctedZValue;
//~ }


//~ gboolean motion_updatePosition()
//~ {
	//~ // Get gyro reading, invert if we are on the yellow side.
	//~ double gyroZ = (blueSide ? 1 : -1) * getCorrectedGyroZReading();

	//~ // Get time elapsed since last call
	//~ double DT = g_timer_elapsed(start,NULL);
	//~ g_timer_start(start);

	//~ // Get new encoder value
    //~ oldEncoder[0] = encoder[0];
    //~ oldEncoder[1] = encoder[1];
    //~ encoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
    //~ encoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);

    //~ // Sometimes, there are communication errors leading to a 0 encoder value: this loop forces a new read to take place.
    //~ int retry = 10;
    //~ while (abs(oldEncoder[LEFT] -encoder[LEFT])  > 100 && retry > 0)
    //~ {
		//~ encoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);
		//~ retry--;
	//~ }
    //~ retry = 10;
    //~ while (abs(oldEncoder[RIGHT] -encoder[RIGHT])  > 100 && retry > 0)
    //~ {
		//~ encoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
		//~ retry--;
	//~ }

    //~ // Compute estimated angle thanks to encoders.
    //~ double deltamm[2];
    //~ deltamm[RIGHT] = (encoder[RIGHT] - oldEncoder[RIGHT]) * DIMENSIONS_STEP_TO_MM;
    //~ deltamm[LEFT] = (encoder[LEFT] - oldEncoder[LEFT]) * DIMENSIONS_STEP_TO_MM;
	//~ double dist =1 / 2.0 * (deltamm[LEFT] + deltamm[RIGHT]);

	//~ // Lock between current_getTheta and end of estimation, to prevent a resetKalman from occuring in this interval.
	//~ g_mutex_lock(&motionKalmanMutex);
	//~ double newAngleEncoder = current_getTheta() + (blueSide	 ? 1 : -1) * atan((deltamm[RIGHT] - deltamm[LEFT]) / DIMENSIONS_DISTANCE_BETWEEN_WHEELS);
	//~ // Get new angle estimate and update robot position.
	//~ double angle = kalman_updateEstimate(&motionKalman, newAngleEncoder, gyroZ, DT);
	//~ g_mutex_unlock(&motionKalmanMutex);

	//~ current_setTheta(angle);
	//~ current_incX(dist * cos(angle));
	//~ current_incY(dist * sin(angle));


	//~ // Log data
	//~ // If it's the first time, open the file
	//~ if(logFile == NULL)
	//~ {
		//~ logFile = g_io_channel_new_file("log.csv", "w", NULL);
		//~ gchar *line = g_strdup_printf("Movement Log: %s\n", ROBOT_NAME);
		//~ g_io_channel_write_chars(logFile, line, -1, NULL, NULL);
		//~ g_io_channel_flush (logFile, NULL);
		//~ g_free(line);

		//~ g_io_channel_write_chars(logFile, "time,encoderRight,encoderLeft,gyro,encoderAngleInc,encoderAngle,gyroAngle,kalmanAngle,accelX,accelY,accelZ\n", -1, NULL, NULL);
		//~ g_io_channel_flush (logFile, NULL);
	//~ }

	//~ logTime +=DT;
	//~ double encoderIncrement = (blueSide	 ? 1 : -1) * atan((deltamm[RIGHT] - deltamm[LEFT]) / DIMENSIONS_DISTANCE_BETWEEN_WHEELS);
	//~ logAngleGyro += gyroZ * DT;
	//~ logAngleEncoder += encoderIncrement;
	//~ double ax, ay, az;
	//~ imu_accelGetValues(imu, &ax, &ay, &az);
	//~ gchar *line = g_strdup_printf("%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f\n", logTime, encoder[RIGHT],  encoder[LEFT],
								  //~ gyroZ, encoderIncrement, logAngleEncoder, logAngleGyro, angle, ax, ay, az);
	//~ g_io_channel_write_chars(logFile, line, -1, NULL, NULL);
	//~ g_io_channel_flush (logFile, NULL);
	//~ g_free(line);
	//~ // Return TRUE to keep timeout alive.
	//~ return TRUE;
//~ }


void *motion_startController()
{
	// Set current target to stop.
	RobotTarget target;
	target.type = STOP;
	robot_setTarget(target);

	// Grab SIGINT (Ctrl + C) and SIGTERM signal in order to stop the motors when this thread is stopped.
	signal(SIGINT, killMotion);
	signal(SIGTERM, killMotion);
	isMotionControllerRunning = TRUE;
	printf("Motion controller started.\n");

	// Create log file

	gchar *date = g_date_time_format(g_date_time_new_now_local(), "%Y%m%dT%H%M%SZ");
	gchar *filename = g_strdup_printf("log%s.csv", date);
	GIOChannel *logFile = g_io_channel_new_file(filename, "w", NULL);
	gchar *line = g_strdup_printf("Robot Log: %s\n", date);
	g_free(date);
	g_free(filename);

	g_io_channel_write_chars(logFile, line, -1, NULL, NULL);
	g_io_channel_flush (logFile, NULL);
	g_free(line);

	g_io_channel_write_chars(logFile, "time,commandVelocityR,commandVelocityL,encoderR,encoderL,gyroX,gyroY,gyroZ,accelX,accelY,accelZ,mouseX,mouseY\n", -1, NULL, NULL);
	g_io_channel_flush (logFile, NULL);

	// Start timer and enter update loop.
	GTimer *motionTimer = g_timer_new();
	double lastTime = 0;
	double currentTime = 0;
	int encoder[2] = {0,0};

	g_timer_start(motionTimer);
	while(isMotionControllerRunning)
	{
		// Compute new motor input.
		// Run position estimator.
		// TODO

		// Log data
		int oldEncoder[2];
		oldEncoder[0] = encoder[0];
		oldEncoder[1] = encoder[1];
		encoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
		encoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);
		// Sometimes, there are communication errors leading to a 0 encoder value: this loop forces a new read to take place.
		int retry = 10;
		while (abs(oldEncoder[LEFT] - encoder[LEFT])  > 100 && retry > 0)
		{
			encoder[LEFT] = L6470_getPosition(robotMotors[LEFT]);
			retry--;
		}
		retry = 10;
		while (abs(oldEncoder[RIGHT] - encoder[RIGHT])  > 100 && retry > 0)
		{
			encoder[RIGHT] = L6470_getPosition(robotMotors[RIGHT]);
			retry--;
		}
		double speedR = L6470_getSpeed(robotMotors[RIGHT]);
		double speedL = L6470_getSpeed(robotMotors[LEFT]);

		double gyroX, gyroY, gyroZ, accelX, accelY, accelZ;
		imu_gyroGetValues(robotIMU, &gyroX, &gyroY, &gyroZ);
		imu_accelGetValues(robotIMU, &accelX, &accelY, &accelZ);
		double mouseX, mouseY;
		//~ ADNS9800_getMotion(robotMouseSensor, &mouseX, &mouseY);
		gchar *line = g_strdup_printf("%f,%f,%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f\n",
		                             g_timer_elapsed(motionTimer, NULL),
		                             speedR, speedL,
		                             encoder[RIGHT],  encoder[LEFT],
		                             gyroX, gyroY, gyroZ,
		                             accelX, accelY, accelZ,
		                             mouseX, mouseY);

		g_io_channel_write_chars(logFile, line, -1, NULL, NULL);
		g_io_channel_flush (logFile, NULL);
		g_free(line);


		// Wait for loop to match loop frequency.
		currentTime = g_timer_elapsed(motionTimer, NULL);
		while(currentTime - lastTime < LOOP_PERIOD)
		{
			g_usleep(10);
			currentTime = g_timer_elapsed(motionTimer, NULL);
		}
		lastTime = currentTime;
	}
	stopMotors();
	printf("Motion controller ended.\n");
	g_io_channel_shutdown(logFile, TRUE, NULL);
	return 0;
}

void motion_stopController()
{
	isMotionControllerRunning = FALSE;
	// Leave 10 extra ms to make sure the motors have time to stop.
	g_usleep(1000000 * LOOP_PERIOD + 10000);
}
