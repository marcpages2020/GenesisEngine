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

#include <string>

struct json_array_t;
typedef json_array_t JSON_Array;

struct json_value_t;
typedef json_value_t JSON_Value;

struct json_object_t;
typedef json_object_t JSON_Object;

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

	const char* engine_name;

public:

	Application(int argc, char* args[]);
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	bool LoadConfig(JSON_Object* object);

public:
	float GetFPS();
	float GetLastDt();
	int GetFPSCap();
	void SetFPSCap(int fps_cap);

	HardwareSpecs GetHardware();
	const char* GetEngineVersion();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

private:
	int	   argc;
	char** args;

	Timer	ms_timer;
	float	dt;
	float	fps;
	float	capped_ms;
	std::vector<Module*> modules_vector;

	const char* version;

	const char* config_path;
};

extern Application* App;

#endif 