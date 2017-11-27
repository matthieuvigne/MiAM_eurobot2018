/// \file MainLoop.c
/// \brief This file implements the main function, as well as several other features.
///
/// \details We use the glib for event handling. The main loop thus consists in initializing the robot,
///			 starting the auxiliary threads, and then running a GMainLoop with timeout functions.
///			 One timeout function is responsible for checking the infrared sensors, and another runs the Kalman filter.
///
///			 This file includes:
///				- The main() function.
///				- Functions to check on the infrare sensors.
///				- The heartbeat thread.
///				- The timer thread.

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <glib.h>
#include "Strategy.h"
#include "Motion.h"
#include <BBBEurobot/KalmanFilter.h>
#include "Global.h"
#include "Initialize.h"

#include "Config.h"



/// \brief Reads data from an analog input.
///
/// \param[in] pin Analog input number (from 0 to 6) of the Beaglebone pin to read.
/// \returns The value of the corresponding input pin (range 0-4095), -1 on failure.
int readADC(int pin)
{
	gchar *fileName = g_strdup_printf("/sys/devices/ocp.3/helper.12/AIN%d", pin);

	GIOChannel *slot = g_io_channel_new_file(fileName, "r", NULL);
	// Could not create file because ADC are not enabled.
	if (slot == NULL)
	{
		printf("Error reading analog input: file %s. Probably ADC are not enabled\n", fileName);
		return -1;
	}
	// Read line input and convert it to an int
	gchar *line = NULL;
	for(int x = 0; x < 5; x++)
		if(line == NULL)
			g_io_channel_read_line (slot, &line, NULL, NULL, NULL);
	g_io_channel_shutdown(slot, FALSE, NULL);
	if(line == NULL)
	{
		printf("Error reading analog input: file %s. Probably ADC are not enabled\n", fileName);
		return -1;
	}
	int value = g_ascii_strtoll(line, NULL, 10);
	g_free(fileName);
	g_free(line);
	return value;
}


// Variables to store weather or not the last time we checked, we saw the opponent's robot.
gboolean oldSensorBack = FALSE;
gboolean oldSensorFront = FALSE;

/// \brief Check the infrared sensors for an obstacle
/// \details This function is called by the main loop in a timeout.
/// \return TRUE, to continue the timeout.
gboolean checkInfrarouge()
{
	// Only one sensor at the back of the robot:
	gboolean sensorBack = readADC(CAPE_ANALOG[IR_BACK]) > IR_BACK_DETECTION;

	// We consider we have seen an obstacle if it has been detected twice in a row.
	// A lone detection is thus considered as noise and ignored.
	detectionBack = sensorBack && oldSensorBack;
	oldSensorBack = sensorBack;

	// Same thing at the front, execpt this time we might have two sensors.
	gboolean sensorFront = readADC(CAPE_ANALOG[IR_FRONT[0]]) > IR_FRONT_DETECTION;
	if(IR_FRONT[1] >= 0)
		sensorFront = sensorFront || readADC(CAPE_ANALOG[IR_FRONT[1]]) > IR_FRONT_DETECTION;
	detectionFront = sensorFront && oldSensorFront;
	oldSensorFront = sensorFront;

	if(detectionFront)
		ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_FRONT, 127);
	else
		ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_FRONT, 0);

	if(detectionBack)
		ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_BACK, 127);
	else
		ledDriver_setLedBrightness(&leddriver, ROOF_OBSTACLE_BACK, 0);

	return TRUE;
}

// Time to stop the robot at the end of the match.
void *timer()
{
	ledDriver_setLedBrightness(&leddriver, ROOF_DEBUG_INFO, 200);
	GTimer *time = g_timer_new ();
	g_timer_start (time);
	printf("Match start\n");
	gulong micro;
	gdouble sec = g_timer_elapsed (time, &micro);

	while((sec * 1000000 + micro) < 90000000)
	{
		g_usleep(5000);
		sec = g_timer_elapsed (time, &micro);
	}
	printf("Match end\n");
	ledDriver_setLedBrightness(&leddriver, ROOF_DEBUG_INFO, 0);
	pthread_kill(strategyThread, SIGINT);
	g_usleep(500000);
	startegy_funnyAction();

	exit(0);
	return 0;
}

// Heartbeat thread to know that the program is still running.
void *heartbeat()
{
	while(TRUE)
	{
		g_usleep(200000);
		ledDriver_setLedBrightness(&leddriver, ROOF_HEARTBEAT, 0);
		g_usleep(200000);
		ledDriver_setLedBrightness(&leddriver, ROOF_HEARTBEAT, 127);
	}
}

// Prints the average value of the gyro on all axis.
void getGyroBiais()
{
	printf("Computing average gyro values, the robot must not move during this.\n");
	double average[3];
	imu_gyroGetValues(imu, &average[0], &average[1], &average[2]);
	int nIter = 1;
	while(TRUE)
	{
		nIter++;
		double readings[3];
		imu_gyroGetValues(imu, &readings[0], &readings[1], &readings[2]);
		for(int i = 0; i < 3; i++)
			average[i] = average[i] * (nIter-1) / nIter + readings[i] / nIter;
		printf("x:%f\ty:%f\tz:%f\r", average[0], average[1], average[2]);
	}
}

int main(int argc, char **argv)
{
	// Show which robot is running this code.
	printf("Running code for: %s\n", ROBOT_NAME);
	// Create a GMainLoop
	GMainLoop* loop = g_main_loop_new( 0, 0 );

	// Initialisation of all the elements on the robot.
	init_MainStartup();

	// Used to get the average bias of the gyro, that must be corrected.
	//~ getGyroBiais();

	// Start the heartbeat thread.
	g_thread_new("Heartbeat", heartbeat, NULL);

	// Wait for the jack cable to be plugged in, then for the match to start.
	ledDriver_setRGBledBrightness(&leddriver, ROOF_RGB_TEAM, 160,0,0);
	init_waitForJackPlug();
	init_waitForStart();
	//~ printf("Press enter to start robot...");
	//~ getchar();
	//~ blueSide = TRUE;

	// Start timer
	start = g_timer_new();
	g_timer_start(start);
	g_thread_new("Timer", timer, NULL);

	// Set robot to initial position
	current_set(STARTING_POSITION);
		
	kalman_init(&motionKalman, current_getTheta());

	// Launch the strategy thread
	g_thread_new("Strategy", strategy_runMatch, NULL);

	// Timeout functions to check on the infrared sensors and update the Kalman estimate.
	g_timeout_add(40, checkInfrarouge, NULL);
	g_timeout_add(20, motion_updatePosition, NULL);

    g_main_loop_run( loop );
	return 0;
}

