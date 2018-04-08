/// \file MainLoop.c
/// \brief This file implements the main function, as well as several other features.
///
/// \details We use the glib for event handling. The main loop thus consists in initializing the robot,
///			 starting the auxiliary threads, and then running a GMainLoop with timeout functions.
///			 One timeout function is responsible for checking the infrared sensors - camera wifi communication will
///			 also be done by a callback function.
///
///			 This file includes:
///				- The main() function.
///				- Functions to check on the infrareds sensors.
///				- The heartbeat thread.
///				- The timer thread.

#include "Robot.h"
#include "MotionController.h"
#include "Localisation.h"
#include "Strategy.h"


// Variables to store weather or not the last timerMain we checked, we saw the opponent's robot.
gboolean oldSensorBack = FALSE;
gboolean oldSensorFront = FALSE;

/// \brief Check the infrared sensors for an obstacle
/// \details This function is called by the main loop in a timeout.
/// \return TRUE, to continue the timeout.
gboolean checkInfrarouge()
{
	//~ // Only one sensor at the back of the robot:
	//~ gboolean sensorBack = gpio_analogRead(CAPE_ANALOG[IR_BACK]) > IR_BACK_DETECTION;

	//~ // We consider we have seen an obstacle if it has been detected twice in a row.
	//~ // A lone detection is thus considered as noise and ignored.
	//~ detectionBack = sensorBack && oldSensorBack;
	//~ oldSensorBack = sensorBack;

	//~ // Same thing at the front, execpt this timerMain we might have two sensors.
	//~ gboolean sensorFront = readADC(CAPE_ANALOG[IR_FRONT[0]]) > IR_FRONT_DETECTION;
	//~ if(IR_FRONT[1] >= 0)
		//~ sensorFront = sensorFront || readADC(CAPE_ANALOG[IR_FRONT[1]]) > IR_FRONT_DETECTION;
	//~ detectionFront = sensorFront && oldSensorFront;
	//~ oldSensorFront = sensorFront;

	//~ if(detectionFront)
		//~ ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_FRONT, 127);
	//~ else
		//~ ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_FRONT, 0);

	//~ if(detectionBack)
		//~ ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_BACK, 127);
	//~ else
		//~ ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_BACK, 0);

	return TRUE;
}


GTimer *timerMain;

// Function that stops the robot.
gboolean stop_robot()
{
	printf("End of match, time: %f\n", g_timer_elapsed(timerMain, NULL));
	// Kill startegy thread to stop the motors.
	pthread_kill(strategyThread, SIGINT);
	g_usleep(500000);
	exit(0);

	return TRUE;
}

// Heartbeat thread.
// This thread simply blinks a cape led, to show that the code is still running
void *heartbeatThread()
{
	while(TRUE)
	{
		g_usleep(200000);
		gpio_digitalWrite(CAPE_LED[0], 1);
		g_usleep(200000);
		gpio_digitalWrite(CAPE_LED[0], 0);
	}
}

gboolean initRobot()
{
	gboolean isInitSuccessful = TRUE;
	gboolean initLCD = lcd_initDefault(&robotLCD, &I2C_2);
	if(!initLCD)
	{
		printf("Could not detect LCD display\n");
		// Can't do much more that turning on the led if we don't have the LCD display.
		gpio_digitalWrite(CAPE_LED[1], 1);
		g_usleep(1000000);
	}
	isInitSuccessful &= initLCD;

	gboolean initIMU = imu_initDefault(&robotIMU, &I2C_1, FALSE);
	if(!initIMU)
	{
		printf("Could not detect IMU\n");
		lcd_setText(robotLCD, "IMU init failed", 0);
		lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		g_usleep(1000000);
	}
	isInitSuccessful &= initIMU;

	gboolean initServo = maestro_initDefault(&robotServo, "/dev/ttyO4");
	if(!initServo)
	{
		printf("Could not talk to servo driver\n");
		lcd_setText(robotLCD, "Servo init failed", 0);
		lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		g_usleep(1000000);
	}
	//~ else
		//~ servo_initPosition();
	isInitSuccessful &= initServo;

	gboolean initMouse = ANDS9800_init(&robotMouseSensor, SPI_0);
	if(!initMouse)
	{
		printf("Could not detect mouse sensor\n");
		lcd_setText(robotLCD, "Mouse init failed", 0);
		lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		g_usleep(1000000);
	}
	isInitSuccessful &= initMouse;

	gboolean initColor = colorSensor_init(&robotColorSensor, &I2C_1);
	if(!initColor)
	{
		printf("Could not detect color sensor\n");
		lcd_setText(robotLCD, "Color init failed", 0);
		lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		g_usleep(1000000);
	}
	else
	{
		// Setup sensor
		colorSensor_setGain(robotColorSensor, TCS34725_GAIN_60X);
		colorSensor_setIntegrationTime(robotColorSensor, 10);
	}
	isInitSuccessful &= initColor;

	// Try motor init - we are more likely to get a failure here, so print in on the first line.
	gboolean initMotor = motion_initMotors();
	if(!initMotor)
	{
		printf("Could not init motors: are they turned on ?\n");
		lcd_setText(robotLCD, "Motor init failed", 0);
		lcd_setBacklight(robotLCD, TRUE, TRUE, FALSE);
	}
	isInitSuccessful &= initMotor;

	if(isInitSuccessful)
	{
		printf("Init successful\n");
		lcd_setText(robotLCD, "Robot init done", 0);
		lcd_setBacklight(robotLCD, FALSE, TRUE, FALSE);
	}
	g_usleep(1000000);
	return isInitSuccessful;
}

// Wait for match start, returns robot side (TRUE = RIGHT, FALSE = LEFT)
gboolean waitForStart(gboolean isInitDone)
{
	// Wait for staring jack to be plugged in.
	if(gpio_digitalRead(CAPE_DIGITAL[0]) == 1)
	{
		printf("Waiting for jack to be plugged in\n");
		lcd_setText(robotLCD, "Waiting for jack", 1);
		while(gpio_digitalRead(CAPE_DIGITAL[0]) == 1)
			g_usleep(50000);
		printf("Jack plugged in, waiting for match start\n");
	}
	if(isInitDone)
		lcd_setText(robotLCD, "Ready to start", 0);

	gboolean isRightSide = FALSE;
	gboolean oldSide = FALSE;
	lcd_setText(robotLCD, "     GREEN     >", 1);
	lcd_setBacklight(robotLCD, FALSE, TRUE, FALSE);

	while(gpio_digitalRead(CAPE_DIGITAL[0]) == 0)
	{
		if(!isInitDone)
		{
			// Retry motor init.
			isInitDone = motion_initMotors();
			if(isInitDone)
			{
				servo_initPosition();
				printf("Motor init successful\n");
				lcd_setText(robotLCD, "Motor init done", 0);
				g_usleep(1000000);
				lcd_setText(robotLCD, "Ready to start", 0);
			}
		}
		if(lcd_isButtonPressed(robotLCD, LCD_BUTTON_RIGHT))
		{
			oldSide = isRightSide;
			isRightSide = TRUE;
		}

		if(lcd_isButtonPressed(robotLCD, LCD_BUTTON_LEFT))
		{
			oldSide = isRightSide;
			isRightSide = FALSE;
		}

		if(oldSide != isRightSide)
		{
			if(isRightSide)
			{
				lcd_setText(robotLCD, "<    ORANGE     ", 1);
				lcd_setBacklight(robotLCD, TRUE, TRUE, FALSE);
			}
			else
			{
				lcd_setText(robotLCD, "     GREEN     >", 1);
				lcd_setBacklight(robotLCD, FALSE, TRUE, FALSE);
			}
		}
		g_usleep(50000);
	}
	return isRightSide;
}

int main(int argc, char **argv)
{
	// Init beaglebone.
	BBB_enableCape();
	// Create a GMainLoop
	GMainLoop* loop = g_main_loop_new(0, 0 	);
	// Start heartbeat thead.
	g_thread_new("Heartbeat", heartbeatThread, NULL);
	// Init robot hardware.
	gboolean initDone = initRobot();
	// Wait for match to start, redoing motor init if previously failed.
	robot_isOnRightSide = waitForStart(initDone);

	// Start match, set timer to 100s.
	timerMain = g_timer_new();
	g_timer_start(timerMain);
	g_timeout_add(100000, stop_robot, NULL);

	// Set robot to initial position: right of the zone, back against the wall.
	startingPosition.x = 400 - ROBOT_WIDTH / 2.0;
	startingPosition.y = -BALL_LENGTH_OFFSET;
	startingPosition.theta = - G_PI_2;
	robot_setPosition(startingPosition);

	// Launch the strategy thread
	g_thread_new("Strategy", strategy_runMatch, NULL);
	g_thread_new("Localisation", localisation_start, NULL);

	// Timeout functions to check on the infrared sensors.
	g_timeout_add(40, checkInfrarouge, NULL);
	// Run glib main loop.
    g_main_loop_run(loop);
	return 0;
}

