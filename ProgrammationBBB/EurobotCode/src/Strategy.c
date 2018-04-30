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
	motion_stopMotors();
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
	for(int x = 0; x< 4; x++)
	{
		motion_translate(-15, FALSE);
		servo_ballDirectionCenter();
		motion_translate(15, FALSE);
		servo_ballDirectionCanon();
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

	// Go the ball catcher under the first tube.
	targetPosition.y += 100;
	motion_goTo(targetPosition, FALSE, TRUE);
	servo_ballDirectionCanon();
	targetPosition.x += 150;
	// Add offset to make the balls a bit on the right of the catching part.
	targetPosition.y = 840 - BALL_WIDTH_OFFSET + 10;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.x = BALL_LENGTH_OFFSET + 100 - 5;
	motion_goTo(targetPosition, TRUE, FALSE);

	gatherWater(TRUE);

	motion_translate(50, TRUE);
	if(robot_isOnRightSide)
		motion_rotate(G_PI_2);
	else
		motion_rotate(G_PI_2 + 0.2);
	throwBalls();

	// Go turn on light swith.
	targetPosition.x = 1130 + 30;
	targetPosition.y += 80;
	motion_goTo(targetPosition, FALSE, TRUE);
	targetPosition.y = CHASSIS_FRONT - 7;
	motion_goTo(targetPosition, FALSE, TRUE);

	RobotPosition resetPosition;
	resetPosition.y = CHASSIS_FRONT;
	resetPosition.theta = G_PI_2;
	localisation_reset(resetPosition, FALSE, TRUE, TRUE);

	// Turn on light switch.
	motion_translate(-110, TRUE);
	servo_clawLightSwitch();
	motion_translate(10, TRUE);

	// Go launch the bee
	motion_translate(-700, TRUE);
	servo_clawUp();

	targetPosition.x = CHASSIS_SIDE + 80 ;
	targetPosition.y = 2000 - (CHASSIS_FRONT + 80);
	motion_goTo(targetPosition, FALSE, TRUE);

	// Recalibrate
	targetPosition.x = CHASSIS_FRONT - 5;
	targetPosition.y = robot_getPositionY();
	motion_goTo(targetPosition, FALSE, TRUE);

	resetPosition.x = CHASSIS_FRONT;
	resetPosition.theta = G_PI;
	localisation_reset(resetPosition, TRUE, FALSE, TRUE);

	if(!robot_isOnRightSide)
		targetPosition.x = CHASSIS_FRONT + 25;
	else
		targetPosition.x = CHASSIS_FRONT + 90;
	motion_goTo(targetPosition, TRUE, TRUE);
	if(!robot_isOnRightSide)
		servo_beeLaunch();
	targetPosition.y = 2000 - CHASSIS_FRONT + 5;
	motion_goTo(targetPosition, FALSE, TRUE);

	resetPosition.y = 2000 - CHASSIS_FRONT;
	resetPosition.theta = -G_PI_2;
	localisation_reset(resetPosition, FALSE, TRUE, TRUE);
	// Launch bee
	if(!robot_isOnRightSide)
	{
		motion_translate(-10, TRUE);
		servo_beeRetract();
	}
	else
		servo_beeLaunch();
	g_usleep(800000);
	servo_beeRetract();
	motion_translate(-50, TRUE);

	while(TRUE) ;;
	// Go grab the cubes.

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
	resetPosition = robot_getPosition();
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

