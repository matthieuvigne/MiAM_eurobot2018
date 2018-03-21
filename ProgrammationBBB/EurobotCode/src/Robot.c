#include "Robot.h"

// Define robot constants.
const double STEP_TO_SI = 2.0 * G_PI * 0.090 / 800.0;
const double WHEEL_SPACING = 0.340;

const int RIGHT = 0;
const int LEFT = 0;

// Implementation of thread-safe robotPosition position and robotPosition target.
RobotPosition robotPosition;
GMutex positionMutex;

double robot_getPositionX()
{
	g_mutex_lock(&positionMutex);
	double x = robotPosition.x;
	g_mutex_unlock(&positionMutex);
	return x;
}

double robot_getPositionY()
{
	g_mutex_lock(&positionMutex);
	double Y = robotPosition.y;
	g_mutex_unlock(&positionMutex);
	return Y;
}

double robot_getPositionTheta()
{
	g_mutex_lock(&positionMutex);
	double t = robotPosition.theta;
	g_mutex_unlock(&positionMutex);
	return t;
}

void robot_setPositionX(double x)
{
	g_mutex_lock(&positionMutex);
	robotPosition.x = x;
	g_mutex_unlock(&positionMutex);
}

void robot_setPositionY(double y)
{
	g_mutex_lock(&positionMutex);
	robotPosition.y = y;
	g_mutex_unlock(&positionMutex);
}

void robot_setPositionTheta(double t)
{
	g_mutex_lock(&positionMutex);
	robotPosition.theta = t;
	g_mutex_unlock(&positionMutex);
}

void robot_setPosition(RobotPosition pos)
{
	g_mutex_lock(&positionMutex);
	robotPosition.x = pos.x;
	robotPosition.y = pos.y;
	robotPosition.theta = pos.theta;
	g_mutex_unlock(&positionMutex);
}

RobotPosition robot_getPosition()
{
	RobotPosition pos;
	g_mutex_lock(&positionMutex);
	pos.x = robotPosition.x;
	pos.y = robotPosition.y;
	pos.theta = robotPosition.theta;
	g_mutex_unlock(&positionMutex);
	return pos;
}
