#pragma once

#include "Timer.h"

struct GnClock
{
	GnClock();
	void Start();
	void Pause();
	void Resume();
	void Reset();
	//time since startup
	float timeSinceStartup();
	void SetDT();

	float deltaTime(); //last frame expressed in seconds time
	float timeScale; //scale at which time is passing
	bool paused;
	float dt;
	Timer timer;
	Timer deltaTimer;
};

namespace Time 
{
	void Init();

	static int frameCount; //frames since game start
	
	static GnClock realClock;
	static GnClock gameClock;
}