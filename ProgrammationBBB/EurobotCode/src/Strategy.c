#include "Robot.h"
#include "Strategy.h"
#include "MotionController.h"
#include "Localisation.h"

// Callaback function called when this thread is killed.
void killStrategy()
{
	printf("Killing strategy thread\n");
	motion_stopMotors();
	// Turn off all servos.
	for(int i = 0; i < 16; i++)
	{
		maestro_setPosition(robotServo, i, 0);
		g_usleep(50);
	}
	motion_stopMotorsHard();
	motion_releaseMotors();
	exit(0);
}

void throwBalls()
{
	servo_ballDirectionCanon();
	servo_startCannon();
	g_usleep(1000000);
	for(int i=0; i < 8; i++)
	{
		servo_millTurn();
		g_usleep(310000);
		servo_millStop();
		g_usleep(300000);
		servo_millTurnBackward();
		g_usleep(80000);

		//~ servo_millTurnBackward();
		//~ g_usleep(200000);
	}
	servo_stopCannon();
	servo_millStop();
}
// Gather water from a water tank.
// This function should be called when the robot is under the water tank: it will open it and handle the balls.
// It returns when no balls have been detected for 0.5s, or if the time elapsed is greater than 5s.
void gatherWater(gboolean sameColor)
{
	motion_setVelocityProfile(1000, 700, 1100);
	if(sameColor)
		servo_ballDirectionCanon();
	else
		servo_ballDirectionCenter();


	servo_closeWaterTank();
	g_usleep(250000);
	motion_translate(-15, FALSE);
	g_usleep(500000);
	//~ motion_shake(25);
	//~ motion_shake(-25);
	motion_translate(20, FALSE);
	motion_translate(-20, FALSE);
	motion_resetVelocityProfile();
}

//< Set robot score on LCD screen.
void robot_setScore(int score)
{
	char displayScore[4];
	sprintf(displayScore, "%03d", score);
	for(int i = 0; i < 3; i++)
		lcd_setChar(robotLCD, displayScore[i], 0, 7 + i);
}

void *strategy_runMatch()
{
	printf("Strategy started\n");
	// Grab SIGINT (Ctrl + C) and SIGTERM signal in order to stop the motors when this thread is stopped.
	signal(SIGINT, killStrategy);
	signal(SIGTERM, killStrategy);
	strategyThread = pthread_self();
	RobotPosition targetPosition = startingPosition;
	RobotPosition resetPosition;
	motion_stopMotorsHard();
	g_usleep(50000);
	int robotScore = 10;
	robot_setScore(robotScore);
	servo_middleWaterTank();

	//~ servo_openClaws();
	//~ g_usleep(2000000);
	//~ servo_closeClaws();
	//~ while(TRUE);

	// Go turn on light swith.
	targetPosition.x = 1130 + 30;
	motion_goTo(targetPosition, FALSE, TRUE);

	targetPosition.x = robot_getPositionX();
	targetPosition.y = CHASSIS_FRONT - 10;
	motion_goTo(targetPosition, FALSE, TRUE);
	resetPosition.y = CHASSIS_FRONT;
	resetPosition.theta = G_PI_2;
	localisation_reset(resetPosition, FALSE, TRUE, TRUE);

	motion_translate(-(250 - CHASSIS_FRONT), TRUE);

	servo_clawLightSwitch();
	g_usleep(1000000);
	motion_translate(10, TRUE);
	servo_clawDown();
	g_usleep(200000);
	robotScore +=25;
	robot_setScore(robotScore);
	motion_translate(-20, TRUE);
	servo_clawUp();

	targetPosition.x = robot_getPositionX();
	targetPosition.y = CHASSIS_FRONT - 25;
	motion_goTo(targetPosition, FALSE, TRUE);
	resetPosition.y = CHASSIS_FRONT;
	resetPosition.theta = G_PI_2;
	localisation_reset(resetPosition, FALSE, TRUE, TRUE);

	// Go launch bee
	gboolean motionSucessful = motion_translate(-470, TRUE);

	// Go launch the bee
	targetPosition.x = CHASSIS_SIDE + 80 ;
	targetPosition.y = 2000 - (CHASSIS_FRONT + 80) - 130;
	if(motionSucessful)
		motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);

	// Recalibrate
	targetPosition.x = CHASSIS_FRONT - 30;
	targetPosition.y = robot_getPositionY();
	if(motionSucessful)
		motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);

	resetPosition.x = CHASSIS_FRONT;
	resetPosition.theta = G_PI;
	localisation_reset(resetPosition, TRUE, FALSE, TRUE);

	if(robot_isOnRightSide)
		targetPosition.x = CHASSIS_FRONT + 60;
	else
		targetPosition.x = CHASSIS_FRONT + 90;
	targetPosition.y = robot_getPositionY();
	if(motionSucessful)
		motionSucessful = motion_goTo(targetPosition, TRUE, TRUE);
	if(motionSucessful)
	{
		if(!robot_isOnRightSide)
			servo_beeLaunch();
		targetPosition.y = 2000 - CHASSIS_FRONT + 10;
		motion_goTo(targetPosition, FALSE, TRUE);

		resetPosition.y = 2000 - CHASSIS_FRONT;
		resetPosition.theta = -G_PI_2;
		localisation_reset(resetPosition, FALSE, TRUE, TRUE);

		// Launch bee
		if(!robot_isOnRightSide)
		{
			motion_translate(-10, TRUE);
			maestro_setSpeed(robotServo, 14, 1100);
			servo_beeRetract();
		}
		else
		{
			servo_beeLaunch();
			g_usleep(800000);
			motion_translate(-10, TRUE);
			g_usleep(300000);
		}
		g_usleep(500000);
		servo_beeRetract();
		robotScore += 50;
		robot_setScore(robotScore);

		motion_translate(-70, TRUE);
	}

	motionSucessful = TRUE;

	// Go grab the balls, ignore the cubes.
	targetPosition.x = 930;
	targetPosition.y = 840 + BALL_WIDTH_OFFSET - 30;
	motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);
	// Go further than what is supposed to, stop at contact
	if(motionSucessful)
	{
		servo_openWaterTank();
		robot_disableIRWater = TRUE;
		targetPosition.x = BALL_LENGTH_OFFSET + 100 - 15;
		stopOnSwitch = TRUE;
		motionSucessful = motion_goTo(targetPosition, TRUE, TRUE);
		stopOnSwitch = FALSE;
		robot_disableIRWater = FALSE;

		if(motionSucessful)
		{
			motion_translate(20, FALSE);
			gatherWater(TRUE);
			robotScore += 10;
			robot_setScore(robotScore);
			motion_translate(60, TRUE);
			if(robot_isOnRightSide)
				motion_rotate(G_PI_2);
			else
				motion_rotate(G_PI_2 + 0.2);
			throwBalls();
			// Number of points scored by throwing the balls: 5points / ball.
			robotScore += 5;
			robot_setScore(robotScore);
		}
	}
	servo_middleWaterTank();

	motionSucessful = TRUE;
	// Go push the cubes
	targetPosition.x = 850;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.x = 810;
	targetPosition.y = CHASSIS_FRONT + 180 - 20;
	motion_setVelocityProfile(700, 300, 800);
	motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);
	motion_resetVelocityProfile();
	if(motionSucessful)
	{
		servo_clawLightSwitch();
		g_usleep(1000000);
		servo_clawDropCube();;
		g_usleep(1000000);
		robotScore += 7;
		robot_setScore(robotScore);
	}
	motionSucessful = TRUE;
	// Go push the second block of cubes.
	motionSucessful = motion_translate(-100, TRUE);
	servo_clawUp();
	targetPosition.x -= 100;
	targetPosition.y = 1500;
	if(motionSucessful)
		motionSucessful = motion_goTo(targetPosition, TRUE, TRUE);

	targetPosition.x= CHASSIS_FRONT - 20;
	if(motionSucessful)
		motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);

	resetPosition.x = CHASSIS_FRONT;
	resetPosition.theta = G_PI;
	localisation_reset(resetPosition, TRUE, FALSE, TRUE);
	if(motionSucessful)
		motionSucessful = motion_translate(-80, TRUE);

	targetPosition.x = 570;
	targetPosition.y = CHASSIS_FRONT + 180 - 30;
	if(motionSucessful)
	{
		motion_setVelocityProfile(700, 300, 800);
		motionSucessful = motion_goTo(targetPosition, FALSE, TRUE);
		motion_resetVelocityProfile();
	}

	if(motionSucessful)
	{
		robotScore += 4;
		robot_setScore(robotScore);
	}
	printf("Strategy ended\n");
	return 0;
}
