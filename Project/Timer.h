#ifndef _CG_TIMER_H_
#define _CG_TIMER_H_

extern "C"
{
#include "glutil.h"
}

/* Simple inline timer class. This class represents a timer that
can be started and stopped at any time, and can tell the user
how much time has passed. */
class Timer
{
public:
	// Constructor, by default the timer is off.
	Timer()
	{
		_startTime = Wallclock();
		_lastStopTime = _startTime;
		_running = false;
	}

	// Retrieves the timer's current elapsed time
	double getTime()
	{
		if (_running)
			return Wallclock() - _startTime;
		else
			return _lastStopTime - _startTime;
	}

	// Starts the timer
	void start()
	{
		if (_running) return;
		_startTime += (Wallclock() - _lastStopTime);
		_running = true;
	}

	// Stops the timer
	void stop()
	{
		if (!_running) return;
		_lastStopTime = Wallclock();
		_running = false;
	}

	// Tells whether the clock is running
	bool isRunning()
	{
		return _running;
	}

private:
	// _startTime is the starting time, although this might
	// shift if the timer is paused. But in any case the 
	// current time - startTime is always the elapsed time.
	double _startTime;

	// The time when the last stop occured.
	double _lastStopTime;

	// Tells whether the timer is currently running.
	bool   _running;
};

#endif
