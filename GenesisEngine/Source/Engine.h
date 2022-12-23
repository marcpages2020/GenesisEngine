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
#include "ModuleEditor.h"

class GnEngine
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;

private:

	Timer	ms_timer;
	float	dt;
	std::vector<Module*> modules_vector;

public:

	GnEngine();
	~GnEngine();

	bool Init();
	update_status Update();
	bool CleanUp();

	int versionMajor;
	int versionMinor;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};