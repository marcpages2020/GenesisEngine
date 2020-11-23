#include "Time.h"

void Time::Init()
{
	frameCount = 0;
	gameClock.timeScale = 1.0f;
	gameClock.paused = true;
	
	realClock.timer.Start();
}


GnClock::GnClock()
{
	dt = 0.33f;
	timeScale = 1.0f;
	paused = false;
}

void GnClock::Start()
{
	paused = false;
	timer.Start();
	timeScale = 1.0f;
}

void GnClock::Pause()
{
	paused = true;
	timeScale = 0.0f;
}

void GnClock::Resume()
{
	paused = false;
	timeScale = 1.0f;
}

void GnClock::Reset()
{
	//GameClock::deltaTime = 0.0f;
	//GameClock::time = 0.0f;
	timeScale = 1.0f;
	paused = false;
}

float GnClock::timeSinceStartup()
{
	return timer.ReadSec();
}

void GnClock::SetDT()
{
	dt = deltaTimer.ReadSec();
}

float GnClock::deltaTime()
{
	return dt * timeScale;
}

/*
float Time::GameClock::time()
{
	return Time::GameClock::timer.ReadSec();
}

float Time::GameClock::deltaTime()
{
	return RealClock::deltaTime * GameClock::timeScale;
}


float Time::RealClock::timeSinceStartup()
{
	return RealClock::timer.ReadSec();
}
*/