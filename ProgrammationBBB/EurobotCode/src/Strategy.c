#include "Robot.h"
#include "Strategy.h"
#include "MotionController.h"

void goToPos(int right, int left)
{
	L6470_goToPosition(robotMotors[0], right);
	L6470_goToPosition(robotMotors[1], left);

	while(L6470_isBusy(robotMotors[0]) != 0 || L6470_isBusy(robotMotors[1]) != 0)
	{
		g_usleep(20000);
		int error = L6470_getError(robotMotors[0]);
		if(error != 0)
			printf("Left motor error: %d\n", error);
		error = L6470_getError(robotMotors[1]);
		if(error != 0)
			printf("Right motor error: %d\n", error);
	}
}


// Callaback function called when this thread is killed.
void killStrategy()
{
	printf("Killing strategy thread\n");
	motion_stopMotors();
	exit(0);
}

void *strategy_runMatch()
{
	printf("Strategy started\n");
	// Grab SIGINT (Ctrl + C) and SIGTERM signal in order to stop the motors when this thread is stopped.
	signal(SIGINT, killStrategy);
	signal(SIGTERM, killStrategy);
	strategyThread = pthread_self();

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

