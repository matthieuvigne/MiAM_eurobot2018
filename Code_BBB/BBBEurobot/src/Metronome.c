#include "BBBEurobot/Metronome.h"

#include <math.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

Metronome metronome_create(int period)
{
	Metronome metronome;
	metronome.nPeriod = period;

    clock_gettime(CLOCK_MONOTONIC, &metronome.startTime);
    metronome.targetTime = metronome.startTime;
    return metronome;
}

void metronome_wait(Metronome *metronome)
{
	metronome->targetTime.tv_nsec += metronome->nPeriod;
	while(metronome->targetTime.tv_nsec >= 1e9)
	{
		metronome->targetTime.tv_nsec -= 1e9;
		metronome->targetTime.tv_sec ++;
	}
	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(metronome->targetTime), NULL);
}

double metronome_getTimeElapsed(Metronome const metronome)
{
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return currentTime.tv_sec - metronome.startTime.tv_sec + (currentTime.tv_nsec - metronome.startTime.tv_nsec) / 1e9;
}
