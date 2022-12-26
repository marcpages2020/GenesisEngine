#pragma once

#include "Globals.h"
#include <vector>
#include "Timer.h"
#include "Module.h"
#include "ModuleHardware.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleAudio.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleRenderer3D.h"

class GnEngine
{
public:
	ModuleHardware* hardware;
	ModuleWindow* window;
	ModuleResources* resources;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleAudio* audio;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleRenderer3D* renderer3D;

private:
	static GnEngine* instance;

	float deltaTime;
	int maxFPS;
	float minFrameMS;

	std::vector<Module*> modulesVector;

public:
	GnEngine();
	~GnEngine();

	static GnEngine* Instance();

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
	bool LoadModulesEditorConfig();
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};