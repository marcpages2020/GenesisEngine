#pragma once

#include "Globals.h"
#include <vector>
#include "Timer.h"
#include "Module.h"
#include "ModuleHardware.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"

class GnEngine
{
public:
	ModuleHardware* hardware;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;

private:
	float deltaTime;
	int maxFPS;
	float minFrameMS;

	std::vector<Module*> modulesVector;

public:

	GnEngine();
	~GnEngine();

	bool Init();
	update_status Update();
	bool CleanUp();

	int versionMajor;
	int versionMinor;

	int GetMaxFPS() const;
	void SetMaxFPS(int newMaxFPS);
	float GetLastDeltaTime() const;
	float GetCurrentFPS() const;

private:
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};