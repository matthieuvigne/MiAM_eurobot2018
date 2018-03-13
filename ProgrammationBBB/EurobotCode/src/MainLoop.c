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
	printf("%f\n", g_timer_elapsed(timerMain, NULL));
	// End motion controller to stop the motors.
	motion_stopController();
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

void initRobot()
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

	//~ gboolean initServo = servoDriver_initDefault(&robotServo, &I2C_1, 27300000);
	//~ if(!initServo)
	//~ {
		//~ printf("Could not detect servo driver\n");
		//~ lcd_setText(robotLCD, "Servo init failed", 0);
		//~ lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		//~ g_usleep(1000000);
	//~ }
	//~ isInitSuccessful &= initServo;

	//~ gboolean initMouse = ANDS9800_init(&robotMouseSensor, SPI_0);
	//~ if(!initMouse)
	//~ {
		//~ printf("Could not detect mouse sensor\n");
		//~ lcd_setText(robotLCD, "Mouse init failed", 0);
		//~ lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		//~ g_usleep(1000000);
	//~ }
	//~ isInitSuccessful &= initMouse;

	//~ gboolean initColor = colorSensor_init(&robotColorSensor, &I2C_2);
	//~ if(!initColor)
	//~ {
		//~ printf("Could not detect color sensor\n");
		//~ lcd_setText(robotLCD, "Color init failed", 0);
		//~ lcd_setBacklight(robotLCD, TRUE, FALSE, FALSE);
		//~ g_usleep(1000000);
	//~ }
	//~ isInitSuccessful &= initColor;

	// Try motor init - we are more likely to get a failure here, so print in on the first line.
	gboolean initMotor = motion_initMotors();
	if(!initMotor)
	{
		printf("Could not init motors: are they turned on ?\n");
		lcd_setText(robotLCD, "Motor init failed", 1);
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
}

void waitForStart()
{
	// TODO
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
	// Should we check and stop if init fails ?
	initRobot();
	// Wait for match to start.
	waitForStart();
	timerMain = g_timer_new();
	g_timer_start(timerMain);
	// Set the robot to stop after 100s (100000ms)
	g_timeout_add(100000, stop_robot, NULL);

	// Set robot to initial position
	//~ robot_setPosition(STARTING_POSITION);
	// Launch the strategy thread
	g_thread_new("Strategy", strategy_runMatch, NULL);
	g_thread_new("MotionController", motion_startController, NULL);

	// Timeout functions to check on the infrared sensors.
	g_timeout_add(40, checkInfrarouge, NULL);
	// Run glib main loop.
    g_main_loop_run(loop);
	return 0;
}

