#include "Application.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new Editor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(editor);
	
	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);
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
	for (int i = 0; i < modules_vector.size(); i++)
	{
		modules_vector[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (int i = 0; i < modules_vector.size(); i++)
	{
		modules_vector[i]->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		modules_vector[i]->PreUpdate(dt);
	}

	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		modules_vector[i]->Update(dt);
	}

	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		modules_vector[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (int i = 0; i < modules_vector.size() && ret == UPDATE_CONTINUE; i++)
	{
		modules_vector[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	modules_vector.push_back(mod);
}