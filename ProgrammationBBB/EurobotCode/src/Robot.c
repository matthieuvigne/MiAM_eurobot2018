#include "Robot.h"

// Implementation of thread-safe robotPosition position and robotPosition target.
RobotPosition robotPosition;
GMutex positionMutex;

RobotTarget robotTarget;
GMutex targetMutex;

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

void robot_setTarget(RobotTarget target)
{
	g_mutex_lock(&targetMutex);
	robotTarget.type = target.type;
	robotTarget.parameter = target.parameter;
	g_mutex_unlock(&targetMutex);
}

RobotTarget robot_getTarget()
{
	RobotTarget target;
	g_mutex_lock(&targetMutex);
	target.type = robotTarget.type;
	target.parameter = robotTarget.parameter;
	g_mutex_unlock(&targetMutex);
	return target;
}
