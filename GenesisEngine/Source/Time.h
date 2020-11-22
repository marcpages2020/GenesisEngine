#pragma once

#include "Timer.h"

namespace Time 
{
	void Init();

	static int frameCount = 0; //frames since game start

	namespace GameClock
	{
		void Start();
		void Pause();
		void Resume();
		void Reset();
		static float time(); //seconds since game start
		float deltaTime(); //last frame time expressed in seconds

		static float timeScale; //scale at which time is passing
		static bool paused;
		static Timer timer;
	}

	namespace RealClock
	{
		//real time since start
		float timeSinceStartup(); 

		static float deltaTime; //last frame expressed in real time
		static Timer timer;
	}
}