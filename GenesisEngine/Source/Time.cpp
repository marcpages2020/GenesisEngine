#include "Time.h"

GnClock Time::realClock;
GnClock Time::gameClock;

int Time::frameCount = 0;

void Time::Init()
{
	gameClock.timeScale = 0.0f;
	gameClock.started = false;
	
	realClock.timer.Start();
	realClock.started = true;
}


GnClock::GnClock()
{
	timeScale = 1.0f;
	paused = false;
}

void GnClock::Start()
{
	paused = false;
	started = true;
	timeScale = 1.0f;
	timer.Start();
}

void GnClock::Stop()
{
	paused = false;
	started = false;
}

void GnClock::Pause()
{
	paused = true;
	timeScale = 0.0f;
	timer.Stop();
}

void GnClock::Resume()
{
	paused = false;
	timeScale = 1.0f;
	timer.Resume();
}

void GnClock::Reset()
{
	timeScale = 1.0f;
	paused = false;
}

void GnClock::Step()
{
	dt = (float)deltaTimer.Read() / 1000 * timeScale;
	deltaTimer.Start();
}

float GnClock::timeSinceStartup()
{
	if (started)
		return timer.ReadSec();
	else
		return 0.0f;
}

float GnClock::deltaTime()
{
	return dt * timeScale;
}
