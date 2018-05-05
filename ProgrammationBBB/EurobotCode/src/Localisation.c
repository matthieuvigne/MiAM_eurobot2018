#include "Robot.h"
#include "MotionController.h"

// Update loop frequency
const double LOOP_PERIOD = 0.010;
gboolean resetLocalisation;
RobotPosition positionReset;
gboolean isXReset, isYReset, isThetaReset;

void localisation_reset(RobotPosition resetPosition, gboolean resetX, gboolean resetY, gboolean resetTheta)
{
	positionReset = resetPosition;
	isXReset = resetX;
	isYReset = resetY;
	isThetaReset = resetTheta;
	resetLocalisation = TRUE;
	g_usleep(1.1 * 1000000 * LOOP_PERIOD);
}

void *localisation_start()
{
	printf("Localisation thread started.\n");
	resetLocalisation = FALSE;

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

	g_io_channel_write_chars(logFile, "time,commandVelocityR,commandVelocityL,encoderR,encoderL,gyroX,gyroY,gyroZ,"
									  "mouseX,mouseY,currentX,currentY,currentTheta\n", -1, NULL, NULL);
	g_io_channel_flush (logFile, NULL);

	// Create kalman filter.
	Kalman kalmanFilter;
	kalman_init(&kalmanFilter, robot_getPositionTheta());
	// Start timer and enter update loop.
	GTimer *localisationTimer = g_timer_new();
	double lastTime = 0;
	double lastLoop = 0;
	double currentTime = 0;
	int oldEncoder[2] = {0,0};

	g_timer_start(localisationTimer);
	while(TRUE)
	{
		// Reset position, if asked for.
		if(resetLocalisation)
		{
			resetLocalisation = FALSE;
			if(isXReset)
				robot_setPositionX(positionReset.x);
			if(isYReset)
				robot_setPositionY(positionReset.y);
			if(isThetaReset)
			{
				robot_setPositionTheta(positionReset.theta);
				kalman_init(&kalmanFilter, positionReset.theta);
			}
		}

		// Get sensor data
		int encoder[2];
		double motorSpeed[2];
		double motorIncrementSI[2];
		for(int i = 0; i < 2; i++)
		{
			encoder[i]= L6470_getPosition(robotMotors[i]);
			motorIncrementSI[i] = (encoder[i] - oldEncoder[i]) * STEP_TO_SI;
			oldEncoder[i] = encoder[i];
			//~ motorSpeed[i]= L6470_getSpeed(robotMotors[i]);
		}

		double gyroX, gyroY, gyroZ;
		imu_gyroGetValues(robotIMU, &gyroX, &gyroY, &gyroZ);
		//~ imu_accelGetValues(robotIMU, &accelX, &accelY, &accelZ);
		gyroZ -= GYRO_Z_BIAS;
		double mouseX, mouseY;
		//~ ADNS9800_getMotion(robotMouseSensor, &mouseX, &mouseY);

		currentTime = g_timer_elapsed(localisationTimer, NULL);
		double dt = currentTime - lastTime;
		lastTime = currentTime;

		// Estimate new position.
		RobotPosition currentPosition = robot_getPosition();
		// Estimate angle.
		double tanTheta = (motorIncrementSI[RIGHT] - motorIncrementSI[LEFT]) / ROBOT_WIDTH;
		// If playing on right side, invert rotation axis to symmetrize robot motion.
		if(robot_isOnRightSide)
		{
			tanTheta = -tanTheta;
			gyroZ = -gyroZ;
		}
		double newAngleEncoder = robot_getPositionTheta() + atan(tanTheta);
		currentPosition.theta = kalman_updateEstimate(&kalmanFilter, newAngleEncoder, gyroZ, dt);

		// Integrate angle estimation on X and Y.
		double linearIncrement = 1 / 2.0 * (motorIncrementSI[LEFT] + motorIncrementSI[RIGHT]);
		currentPosition.x += linearIncrement * cos(currentPosition.theta);
		// Minus sign: the frame is indirect.
		currentPosition.y -= linearIncrement * sin(currentPosition.theta);
		robot_setPosition(currentPosition);

		// Log data
		gchar *line = g_strdup_printf("%f,%f,%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f\n",
		                             currentTime,
		                             motorSpeed[RIGHT], motorSpeed[LEFT],
		                             encoder[RIGHT],  encoder[LEFT],
		                             gyroX, gyroY, gyroZ,
		                             mouseX, mouseY,
		                             currentPosition.x, currentPosition.y, currentPosition.theta);

		g_io_channel_write_chars(logFile, line, -1, NULL, NULL);
		g_io_channel_flush (logFile, NULL);
		g_free(line);

		// Wait for time to match loop frequency.
		currentTime = g_timer_elapsed(localisationTimer, NULL);
		while(currentTime - lastLoop < LOOP_PERIOD)
		{
			g_usleep(10);
			currentTime = g_timer_elapsed(localisationTimer, NULL);
		}
		lastLoop = currentTime;
	}
	return 0;
}
