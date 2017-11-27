/*
 * Strategie.c
 * 
 * Thread executant la strategie du robot
 * 
 */


#include <pthread.h>

#include "Global.h"
#include "Strategy.h"
#include "Motion.h"

#include "Config.h"

// On a kill/interrupt signal, stop the motors before exiting.
void signalEndCode(int signum) 
{
	printf("Strategy killed\n");
    motion_stopMotors();
    // Using exit prevents the funny action from running.
    //~ exit(1);
}

void startegy_funnyAction()
{
	AX12_launchRocket(ax12);
	g_usleep(1000000);
}


void dropCylinder()
{
	
	AX12_grabCylinder(ax12);
	g_usleep(500000);
	ax12_setPosition(ax12, AX12_CYLINDER_FALL, 100); 
	g_usleep(1000000);
	AX12_releaseCylinder(ax12);
	g_usleep(500000);
}

void strategy_servoToInitialPosition()
{ 
	ax12_setSpeed(ax12, AX12_CYLINDER_FALL, 500);
	ax12_setSpeed(ax12, AX12_CYLINDER_GRAB, 500);
	
	AX12_retractCylinderFinger(ax12);
	
	AX12_grabRocket(ax12);
	
	ax12_eeprom_setAngleLimit(ax12, AX12_WHEEL, 0, 1023);
	
	// Move wheel servo to visually check the connection.
	ax12_setPosition(ax12, AX12_WHEEL, 100);
	g_usleep(100000);
	ax12_setPosition(ax12, AX12_WHEEL, 300);
	
	g_usleep(1000000);
	
	// Set wheel servo in continuous rotation mode.
	ax12_eeprom_setAngleLimit(ax12, AX12_WHEEL, 0, 0);
	
	AX12_retractCylinder(ax12);	
}


void *strategy_runMatch() 
{
	// Grab SIGINT (Ctrl + C) and SIGTERM signal in order to stop the motors when killing the code.
	signal(SIGINT, signalEndCode);
	signal(SIGTERM, signalEndCode);
	// Assign strategyThread variable, so that it can be canceled elsewhere
	strategyThread = pthread_self();
	
	printf("Strategy started\n");

	RobotPosition pos = current_get();
		
	pos.y -= 100;
	motion_GoTo(pos, FALSE);
	
	AX12_spinWheel(ax12);
	AX12_extendCylinderFinger(ax12);
	
	pos.x = 1030;
	pos.y = 1500;
	motion_GoTo(pos, FALSE);

	pos.x = 500;
	pos.y = 900;
	motion_GoTo(pos, FALSE);
	
	pos.x = 350;
	pos.y = 1000;
	motion_GoTo(pos, TRUE);
	
	pos.x = 100;
	motion_GoTo(pos, TRUE);
	motion_resetKalman(0);
	current_setX(110 + DIMENSIONS_BACK_OFFSET);
	
	pos.x = 500;
	motion_GoTo(pos, FALSE);
	
	// Position the motor in front of the cylinder
	pos.x = 700;
	pos.y = 800;
	motion_GoTo(pos, TRUE);
	// Drop cylinder, with a change of speed
	motion_stopMotors();
	g_usleep(300000);
	motion_setVelocityProfile(350,400,400);
	g_usleep(100000);
	motion_translation(-240, FALSE);
	motion_stopMotors();
	g_usleep(500000);
	motion_setVelocityProfile(MOTOR_MAX_SPEED, 0.5*MOTOR_MAX_SPEED,0.5*MOTOR_MAX_SPEED);
	
	// Wait for secondary robot to go away.
	AX12_stopWheel(ax12);
	g_usleep(10000000);
	AX12_spinWheel(ax12);
	
	pos.x -= 150;
	pos.y += 150;
	motion_GoTo(pos, FALSE);
		
	pos.x = 1020;
	pos.y = 1000;
	motion_GoTo(pos, FALSE);
	
	pos.y = 1720;
	motion_GoTo(pos, FALSE);
	
	AX12_stopWheel(ax12);
	
	g_usleep(500000);
	
	AX12_reverseWheel(ax12);
	
	g_usleep(500000);
	
	// Go back to drop cylinders, stopping in the middle to go slower.
	motion_translation(-150, TRUE);
	motion_translation(-180, TRUE);
	
	AX12_stopWheel(ax12);
	
	printf("Strategy ended\n");
	return 0;
}

