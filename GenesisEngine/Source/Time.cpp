#include "Time.h"

void Time::Init()
{
	GameClock::timeScale = 1.0f;
	GameClock::paused = true;
	
	RealClock::timer.Start();
}

//GameClock ===========================================================

void Time::GameClock::Start()
{
	paused = false;
	timer.Start();
	timeScale = 1.0f;
}

void Time::GameClock::Pause()
{
	paused = true;
	timeScale = 0.0f;
}

void Time::GameClock::Resume()
{
	paused = false;
	timeScale = 1.0f;
}

void Time::GameClock::Reset()
{
	//GameClock::deltaTime = 0.0f;
	//GameClock::time = 0.0f;
	GameClock::timeScale = 1.0f;
	GameClock::paused = false;
}

float Time::GameClock::time()
{
	return Time::GameClock::timer.ReadSec();
}

float Time::GameClock::deltaTime()
{
	return RealClock::deltaTime * GameClock::timeScale;
}

//RealClock =========================================================

float Time::RealClock::timeSinceStartup()
{
	return RealClock::timer.ReadSec();
}
