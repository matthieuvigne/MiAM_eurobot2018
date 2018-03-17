#include "Robot.h"
#include "Strategy.h"

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

void *strategy_runMatch()
{
	printf("Strategy started\n");

	// Trajecotry for mocap

	// Straight line
	goToPos(1200, 1200);
	g_usleep(500000);
	goToPos(-1200, -1200);
	g_usleep(500000);

	// Rotation
	goToPos(800,-800);
	g_usleep(500000);
	goToPos(-800,800);
	g_usleep(500000);

	//~ // Draw square
	int squareSize = 1500;
	int rotationAngle = 400;

	for(int i = 0; i < 4; i++)
	{
		goToPos(squareSize, squareSize);
		g_usleep(500000);
		goToPos(rotationAngle, -rotationAngle);
		g_usleep(500000);
	}
	//~ // Draw square, turning the other direction

	//~ for(int i = 0; i < 4; i++)
	//~ {
		//~ goToPos(squareSize, squareSize);
		//~ g_usleep(500000);
		//~ goToPos(-rotationAngle, rotationAngle);
		//~ g_usleep(500000);
	//~ }

	printf("Strategy ended\n");
	return 0;
}

