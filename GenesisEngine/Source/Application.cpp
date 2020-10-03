#include "Application.h"
#include "glew/include/glew.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	scene = new ModuleSceneIntro(this);
	editor = new Editor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);

	//AddModule(scene);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);

	int cap = 60;
	capped_ms = 1000 / cap;

	version_major = 0;
	version_minor = 1;
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

	// Call Init() in all modules
	for (int i = 0; i < modules_vector.size() && ret == true; i++)
	{
		ret = modules_vector[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (int i = 0; i < modules_vector.size() && ret == true; i++)
	{
		ret = modules_vector[i]->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000;
	fps = 1.0f / dt;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	Uint32 last_frame_ms = ms_timer.Read();

	if (last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->PreUpdate(dt);
	}

	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->Update(dt);
	}

	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = modules_vector[i]->PostUpdate(dt);
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

	return ret;
}

void Application::AddModule(Module* mod)
{
	modules_vector.push_back(mod);
}

float Application::GetFPS() { return fps; }

float Application::GetLastDt() { return ms_timer.Read() * 10; }

int Application::GetFPSCap()
{
	return 1000 / capped_ms;
}

void Application::SetFPSCap(int fps_cap)
{
	capped_ms = 1000 / fps_cap;
}

HardwareSpecs Application::GetHardware()
{
	HardwareSpecs specs;

	//CPU
	specs.cpu_count = SDL_GetCPUCount();
	specs.cache = SDL_GetCPUCacheLineSize();

	//RAM
	specs.ram = SDL_GetSystemRAM() / 1000;
	
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
	specs.gpu_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	specs.gpu = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

	//specs.vram_budget;
	//specs.vram_usage;
	//specs.vram_available;
	//specs.vram_reserved;

	return specs;
}

void Application::GetEngineVersion(int& g_major, int& g_minor)
{
	g_major = version_major;
	g_minor = version_minor;
}
