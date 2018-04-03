#include "Robot.h"
#include "Strategy.h"
#include "MotionController.h"

// Callaback function called when this thread is killed.
void killStrategy()
{
	printf("Killing strategy thread\n");
	motion_stopMotors();
	// Turn off all servos.
	for(int i = 0; i < 16; i++)
		maestro_setPosition(robotServo, i, 0);
	exit(0);
}

// Gather water from a water tank.
// This function should be called when the robot is under the water tank: it will open it and handle the balls.
// It returns when no balls have been detected for 0.5s, or if the time elapsed is greater than 5s.
void gatherWater()
{
	servo_ballDirectionCenter();
	servo_openWaterTank();
	int noDetect = 0;
	int lastBall = 0;
	GTimer *gatherTime = g_timer_new();
	g_timer_start(gatherTime);
	while(g_timer_elapsed(gatherTime, NULL) < 5.0)
	{
		ColorOutput color = colorSensor_getData(robotColorSensor);
		int ballSide = 0;
		// Detect orange ball
		printf("R %d G %d B %d C %d\n", color.red, color.green, color.blue, color.clear);
		if(color.red > 12000 && color.green > 12000 && color.blue < 8000)
		{
			printf("Orange ball\n");
			ballSide = 1;
		}
		else if(color.red < 8000 && color.green > 12000 && color.blue < 8000)
		{
			printf("Green ball\n");
			ballSide = 1;
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
		}
		// If it's the first time we see a ball, give it some time to go down.
		if(lastBall != ballSide && ballSide != 0)
			g_usleep(200000);
		lastBall = ballSide;

		if(noDetect > 5)
			break;
		g_usleep(100000);
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

	//~ motion_translate(500, TRUE);
	servo_initPosition();
	g_usleep(1000000);

	gatherWater();

	while(TRUE) ;;
	// Go the ball catcher under the first tube.
	targetPosition.y = 840 + BALL_WIDTH_OFFSET;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.x = BALL_LENGTH_OFFSET;
	motion_goTo(targetPosition, TRUE, TRUE);

	while(TRUE) ;;
	// Graps the first balls.

	// Go back to base to shoot.
	targetPosition.x += 80;
	motion_goTo(targetPosition, FALSE, TRUE);

	while(TRUE) ;;
	// Trajecotry for mocap
	motion_rotate(G_PI_2);
	printf("Rotation done\n");
	while(TRUE);;
	// Straight line
	motion_translate(0.5, FALSE);
	g_usleep(500000);
	g_usleep(500000);
	motion_translate(-0.5, FALSE);
	g_usleep(500000);

	// Rotation
	motion_rotate(G_PI_2);
	g_usleep(500000);
	motion_rotate(-G_PI_2);
	g_usleep(500000);

	// Draw square
	double squareSize = 0.5;
	RobotPosition pos = robot_getPosition();
	pos.x += squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.y += squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.x -= squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.y -= squareSize;
	motion_goTo(pos, FALSE, FALSE);

	pos.x += squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.y -= squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.x -= squareSize;
	motion_goTo(pos, FALSE, FALSE);
	pos.y += squareSize;
	motion_goTo(pos, FALSE, FALSE);


	printf("Strategy ended\n");
	return 0;
}

