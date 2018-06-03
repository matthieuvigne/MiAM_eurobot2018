/// \file Metronome.h
/// \brief Precise sleep for periodic event.
///
/// \details This file provides a way to execute code in a periodic manner, as precisesly as possible.
/// The idea is the same as g_timeout_add: we want some code to be executed every delta_t time.
/// However, g_timeout_add is no meant for precise timing, and doesn't catchup execution time.
/// Here, we use clock_nanosleep on TIMER_ABSTIME to be as close as possible to real time, while catching up
/// delays.
/// As a benchmark, I tried calling a funciton every 5ms for 100s:
///  - using g_timeout function call was accurate withing 10%, and we ended up doing only 18600 iterations (7% loss).
///  - using thsi metronome, function call was accurate withing 1%, and exactly 20000 iterations were performed.
///	\note	 All functions in this header should be prefixed with metronome_.
#ifndef METRONOME
#define METRONOME
	#include <time.h>
	typedef struct {
		struct timespec startTime; ///< The start time of the metronome (i.e. time when init was called).
		struct timespec targetTime; ///< The target time to stop sleep: this is equal to startTime + n_iterations * nPeriod
		int nPeriod; ///< Metronome period, in nanoseconds.
	}Metronome;

	/// \brief Create a metronome, at the current time, with the given period.
	///
	/// \param[in] period Metronome period, in nanoseconds.
	/// \return a Metronome struct.
	Metronome metronome_create(int period);

	/// \brief Wait for one period.
	/// \details This funciton sleeps until the duration nPeriod has elapsed since the last call to metronome_wait or
	/// metronome_create. Possibly this will not sleep at all, if the CPU is running late; it will sleep no more than
	/// no more than nPeriod (granted no modification to targetTime is done by the user).
	///
	/// \param[in] metronome Pointer to a metronome struct.
	void metronome_wait(Metronome *metronome);

	/// \brief Get the time elapsed since startTime.
	///
	/// \param[in] metronome A metronome struct.
	/// \return Time since startTime, in seconds.
	double metronome_getTimeElapsed(Metronome const metronome);
#endif
