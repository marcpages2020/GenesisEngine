#include "Application.h"
#include "glew/include/glew.h"
#include "Globals.h"
#include "FileSystem.h"
#include "Time.h"
#include "GnJSON.h"

#include "parson/parson.h"

Application::Application(int argc, char* args[]) : argc(argc), args(args), want_to_save(false), want_to_load(false), in_game(false)
{
	window = new ModuleWindow(true);
	input = new ModuleInput(true);
	renderer3D = new ModuleRenderer3D(true);
	camera = new ModuleCamera3D(true);
	scene = new ModuleScene(true);
	editor = new Editor(true);
	resources = new ModuleResources(true);

	// Main Modules
	AddModule(window);
	AddModule(resources);
	AddModule(camera);
	AddModule(input);
	AddModule(scene);

	// Renderer last!
	AddModule(editor);
	AddModule(renderer3D);

	int cap = 60;
	capped_ms = 1000 / cap;
}

Application::~Application()
{
	std::vector<Module*>::reverse_iterator item = modules_vector.rbegin();

	while(item != modules_vector.rend())
	{
		delete *item;
		++item ;
	}
}

bool Application::Init()
{
	bool ret = true;

	FileSystem::Init();
	Time::Init();

	char* buffer = nullptr;

	uint size = FileSystem::Load("Library/Config/config.json", &buffer);
	GnJSONObj config(buffer);

	engine_name = config.GetString("engineName", "Genesis Engine");
	engine_version = config.GetString("version", "0.0");

	GnJSONArray modules_array(config.GetArray("modules_config"));

	// Call Init() in all modules
	for (size_t i = 0; i < modules_vector.size() && ret == true; i++)
	{
		GnJSONObj module_config(modules_array.GetObjectInArray(modules_vector[i]->name));

		ret = modules_vector[i]->LoadConfig(module_config);
		ret = modules_vector[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (size_t i = 0; i < modules_vector.size() && ret == true; i++)
	{
		ret = modules_vector[i]->Start();
	}
	

	config.Release();
	RELEASE_ARRAY(buffer);

	Time::realClock.deltaTimer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)Time::realClock.deltaTimer.Read() / 1000;
	fps = 1.0f / dt;

	Time::realClock.Step();
	Time::gameClock.Step();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Uint32 last_frame_ms = Time::realClock.deltaTimer.Read();
	if (last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
	}

	Time::frameCount++;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (size_t i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->PreUpdate(dt);
	}

	for (size_t i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->Update(dt);
	}

	for (size_t i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->PostUpdate(dt);
	}
	
	if (!endFrameTasks.empty()) {
		for (size_t i = 0; i < endFrameTasks.size(); i++)
		{
			endFrameTasks.top()->OnFrameEnd();
			endFrameTasks.pop();
		}
	}

	if (want_to_save)
	{
		scene->Save(_file_to_save);
		want_to_save = false;
	}

	if (want_to_load)
	{
		scene->Load(_file_to_load);
		want_to_load = false;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (int i = modules_vector.size() -1; i > 0; i--)
	{
		modules_vector[i]->CleanUp();
	}

	FileSystem::CleanUp();

	return ret;
}

void Application::StartGame()
{
	in_game = true;
	Time::gameClock.Start();
	Save("Library/Scenes/tmp.scene");
}

void Application::StopGame()
{
	in_game = false;
	Time::gameClock.Stop();
	Load("Library/Scenes/tmp.scene");
}

void Application::AddModule(Module* mod)
{
	modules_vector.push_back(mod);
}

float Application::GetFPS() { return fps; }

float Application::GetLastDt() { return dt; }

int Application::GetFPSCap()
{
	return 1000 / capped_ms;
}

void Application::SetFPSCap(int fps_cap)
{
	capped_ms = 1000.0f / (float)fps_cap;
}

void Application::Save(const char* filePath)
{
	want_to_save = true;
	strcpy_s(_file_to_save, filePath);
}

void Application::Load(const char* filePath)
{
	want_to_load = true;
	strcpy_s(_file_to_load, filePath);
}

void Application::AddModuleToTaskStack(Module* callback)
{
	endFrameTasks.push(callback);
}

HardwareSpecs Application::GetHardware()
{
	HardwareSpecs specs;

	//CPU
	specs.cpu_count = SDL_GetCPUCount();
	specs.cache = SDL_GetCPUCacheLineSize();

	//RAM
	specs.ram = SDL_GetSystemRAM() / 1000.0f;
	
	//Caps
	specs.RDTSC = SDL_HasRDTSC();
	specs.MMX = SDL_HasMMX();
	specs.SSE = SDL_HasSSE();
	specs.SSE2 = SDL_HasSSE2();
	specs.SSE3 = SDL_HasSSE3();
	specs.SSE41 = SDL_HasSSE41();
	specs.SSE42 = SDL_HasSSE42();
	specs.AVX = SDL_HasAVX();
	specs.AVX2 = SDL_HasAVX2();
	specs.AltiVec = SDL_HasAltiVec();

	if (specs.RDTSC) { specs.caps += "SDTSC"; }
	if (specs.MMX) { specs.caps += ", MMX"; }
	if (specs.SSE) { specs.caps += ", SSE"; }
	if (specs.SSE2) { specs.caps += ", SSE2"; }
	if (specs.SSE3) { specs.caps += ", SSE3"; }
	if (specs.SSE41) { specs.caps += ", SSE41"; }
	if (specs.SSE42) { specs.caps += ", SSE42"; }
	if (specs.AVX) { specs.caps += ", AVX2"; }
	if (specs.AltiVec) { specs.caps += ", AltiVec"; }

	//GPU
	//GLubyte* vendor = 
	specs.gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	specs.gpu_brand = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

	return specs;
}



