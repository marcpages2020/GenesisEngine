#pragma once

#include "Globals.h"
#include <vector>
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "Editor.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleCamera3D* camera;
	ModuleSceneIntro* scene;
	Editor* editor;
	ModuleRenderer3D* renderer3D;

private:
	Timer	ms_timer;
	float	dt;
	float	fps;
	float	capped_ms;
	std::vector<Module*> modules_vector;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	float GetFPS();
	float GetLastDt();
	int GetFPSCap();
	void SetFPSCap(int fps_cap);

	void GetHardware(int& CPUs, int&cache, int& RAM);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};