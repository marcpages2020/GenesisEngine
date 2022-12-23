// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

void Timer::Resume()
{
	running = true;
	started_at = SDL_GetTicks() - (stopped_at - started_at);
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}

float Timer::ReadSec() const
{
	if (running == true)
	{
		return float(SDL_GetTicks() - started_at) / 1000.0f;
	}
	else
	{
		return float(stopped_at - started_at) / 1000.0f;
	}
}


