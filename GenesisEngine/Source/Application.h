#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Globals.h"
#include <vector>
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "Editor.h"
#include "ModuleResources.h"

#include <string>
#include <stack>

struct HardwareSpecs
{
	int cpu_count;
	int cache;
	float ram;
	const char* gpu;
	const char* gpu_brand;
	float vram_budget;
	float vram_usage;
	float vram_available;
	float vram_reserved;

	bool RDTSC = false;
	bool MMX = false;
	bool SSE = false;
	bool SSE2 = false;
	bool SSE3 = false;
	bool SSE41 = false;
	bool SSE42 = false;
	bool AVX = false;
	bool AVX2 = false;
	bool AltiVec = false;

	std::string caps;
};

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleCamera3D* camera;
	ModuleScene* scene;
	Editor* editor;
	ModuleRenderer3D* renderer3D;
	ModuleResources* resources;

	const char* engine_name;
	const char* engine_version;
	bool in_game;

public:

	Application(int argc, char* args[]);
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void StartGame();
	void StopGame();

	float GetFPS();
	float GetLastDt();
	int GetFPSCap();
	void SetFPSCap(int fps_cap);
	void Save(const char* filePath);
	void Load(const char* filePath);

	void AddModuleToTaskStack(Module* callback);
	HardwareSpecs GetHardware();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

private:
	int	   argc;
	char** args;

	float	dt;
	float	fps;
	float	capped_ms;
	std::vector<Module*> modules_vector;

	const char* config_path;

	bool want_to_save;
	bool want_to_load;

	char _file_to_load[256];
	char _file_to_save[256];

	std::stack<Module*> endFrameTasks;
};

extern Application* App;

#endif 