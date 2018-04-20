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
	exit(0);
}

void throwBalls()
{

	servo_startCannon();
	g_usleep(1000000);
	for(int i=0; i < 7; i++)
	{
		servo_millTurn();
		g_usleep(335000);
		servo_millStop();
		g_usleep(500000);

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
	motion_translate(3, FALSE);
	if(sameColor)
		servo_ballDirectionCanon();
	else
		servo_ballDirectionCenter();

	servo_openWaterTank();

	if(sameColor)
	{
		for(int x = 0; x< 5; x++)
		{
			motion_translate(-20, FALSE);
			motion_translate(20, FALSE);
		}
	}
	else
	{
		int noDetect = 0;
		int lastBall = 0;

		// delay the return to mid position
		int delayedCount = 0;

		GTimer *gatherTime = g_timer_new();
		g_timer_start(gatherTime);
		//~ while(g_timer_elapsed(gatherTime, NULL) < 5.0)
		while(g_timer_elapsed(gatherTime, NULL) < 500.0)
		{
			ColorOutput color = colorSensor_getData(robotColorSensor);
			int ballSide = 0;
			// Detect orange ball
			printf("R %d G %d B %d C %d\n", color.red, color.green, color.blue, color.clear);
			if(color.clear < 6000 && color.red > 1900 && color.blue < 4000 && color.green < 4000)
			{
				printf("Orange ball\n");
				ballSide = 1;
			}
			else if(color.clear < 6000 && color.red < 1900 && color.blue < 4000 && color.green < 4000)
			{
				printf("Green ball\n");
				ballSide = -1;
			}
			if(ballSide == 0)
			{
				servo_ballDirectionCenter();
				noDetect++;
			}
			else
			{
				noDetect = 0;
				if(ballSide == 1)
					servo_ballDirectionBin();
				else
					servo_ballDirectionCanon();
				g_usleep(550000);
				servo_ballDirectionCenter();
			}
			// If it's the first time we see a ball, give it some time to go down.
			if(lastBall != ballSide && ballSide != 0)
				g_usleep(200000);
			lastBall = ballSide;

			//~ if(noDetect > 5)
				//~ break;
			g_usleep(100000);
		}
	}

}

void *strategy_runMatch()
{
	printf("Strategy started\n");
	// Grab SIGINT (Ctrl + C) and SIGTERM signal in order to stop the motors when this thread is stopped.
	signal(SIGINT, killStrategy);
	signal(SIGTERM, killStrategy);
	strategyThread = pthread_self();
	RobotPosition targetPosition = startingPosition;

	servo_beeLaunch();
	while(TRUE) ;;
	//~ throwBalls();
	//~ servo_openWaterTank();
	//~ while(TRUE);;
	//~ motion_rotate(G_PI);
	//~ motion_rotate(0.2);
	//~ motion_translate(500, TRUE);
	//~ while(TRUE) ;;
	//~ while(TRUE)
	//~ {
		//~ servo_millTurn();
	//~ }
	//~ g_usleep(100000000);
	//~ servo_millStop();

	//~ while(TRUE) ;;

	//~ servo_trayDown();
	//~ g_usleep(1000000);
	//~ servo_trayUp();

	//~ gatherWater();
	//~ motion_translate(1000, FALSE);
	//~ motion_rotate(-G_PI_2);

	//~ servo_beeLaunch();
	//~ while(TRUE);;
	//~ gatherWater(TRUE);
	//~ throwBalls();
	//~ while(TRUE) ;;
	// Go the ball catcher under the first tube.
	targetPosition.y += 100;
	motion_goTo(targetPosition, FALSE, TRUE);
	servo_ballDirectionCanon();
	targetPosition.x += 150;
	// Add offset to make the balls a bit on the right of the catching part.
	targetPosition.y = 840 - BALL_WIDTH_OFFSET;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.x = BALL_LENGTH_OFFSET + 100 - 10;
	motion_goTo(targetPosition, TRUE, FALSE);

	g_usleep(1000000);
	gatherWater(TRUE);
	while(TRUE) ;;
	//~ while(TRUE) ;;

	// Graps the first balls.

	// Go around the cubes to launch the bee.
	targetPosition.x += 500;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.y = 1530;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.x = CHASSIS_FRONT + 10;
	motion_goTo(targetPosition, FALSE, TRUE);
	// Hit table side so as to reset the robot position.
	motion_translate(20, FALSE);
	RobotPosition resetPosition = robot_getPosition();
	resetPosition.x = CHASSIS_FRONT;
	resetPosition.theta = G_PI;
	localisation_reset(resetPosition, TRUE, FALSE, TRUE);

	motion_translate(-50, TRUE);
	targetPosition.y = 2000 - CHASSIS_FRONT;
	motion_goTo(targetPosition, FALSE, TRUE);

	// Hit table side so as to reset the robot position.
	motion_translate(20, FALSE);
	resetPosition = robot_getPosition();
	resetPosition.y = 2000 - CHASSIS_FRONT;
	resetPosition.theta = - G_PI_2;
	localisation_reset(resetPosition, FALSE, TRUE, TRUE);
	// Launch bee.

	// Go back to grasp first three cubes.
	motion_translate(-100, TRUE);
	targetPosition.x = 300;
	targetPosition.y = 1700;
	motion_goTo(targetPosition, FALSE, TRUE);

	// Lower claw, then grasp cubes.
	targetPosition.y = 1190 + 60 + CLAW_OFFSET;
	motion_goTo(targetPosition, FALSE, TRUE);

	// Raise claw, go back to base to set cubes and launch balls.

	targetPosition.x = 200;
	targetPosition.y = 200;
	motion_goTo(targetPosition, FALSE, TRUE);

	printf("Strategy ended\n");
	return 0;
}

