#include "Global.h" 

// Current robot position, and mutex to have thread-safe access.
RobotPosition current;
GMutex mut;

double current_getX()
{
	g_mutex_lock(&mut);
	double x = current.x;
	g_mutex_unlock(&mut);
	return x;
}

double current_getY()
{
	g_mutex_lock(&mut);
	double Y = current.y;
	g_mutex_unlock(&mut);
	return Y;
}

double current_getTheta()
{
	g_mutex_lock(&mut);
	double t = current.theta;
	g_mutex_unlock(&mut);
	return t;
}


void current_setX(double x)
{
	g_mutex_lock(&mut);
	current.x = x;
	g_mutex_unlock(&mut);
}

void current_setY(double y)
{
	g_mutex_lock(&mut);
	current.y = y;
	g_mutex_unlock(&mut);
}

void current_setTheta(double t)
{
	g_mutex_lock(&mut);
	current.theta = t;
	g_mutex_unlock(&mut);
}

void current_incX(double x)
{
	g_mutex_lock(&mut);
	current.x += x;
	g_mutex_unlock(&mut);
}

void current_incY(double y)
{
	g_mutex_lock(&mut);
	current.y += y;
	g_mutex_unlock(&mut);
}

void current_set(RobotPosition pos)
{
	current_setX(pos.x);
	current_setY(pos.y);
	current_setTheta(pos.theta);
}
RobotPosition current_get()
{
	RobotPosition pos;
	pos.x = current_getX();
	pos.y = current_getY();
	pos.theta = current_getTheta();
	return pos;
}
