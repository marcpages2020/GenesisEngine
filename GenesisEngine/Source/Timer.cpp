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
	startedAt = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stoppedAt = SDL_GetTicks();
}

void Timer::Resume()
{
	running = true;
	startedAt = SDL_GetTicks() - (stoppedAt - startedAt);
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if (running == true)
	{
		return SDL_GetTicks() - startedAt;
	}
	else
	{
		return stoppedAt - startedAt;
	}
}

float Timer::ReadSec() const
{
	if (running == true)
	{
		return float(SDL_GetTicks() - startedAt) / 1000.0f;
	}
	else
	{
		return float(stoppedAt - startedAt) / 1000.0f;
	}
}


