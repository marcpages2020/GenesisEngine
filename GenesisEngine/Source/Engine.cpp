#include "Engine.h"

GnEngine::GnEngine() : versionMajor(0), versionMinor(1)
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);
}

GnEngine::~GnEngine()
{
	std::vector<Module*>::reverse_iterator item = modules_vector.rbegin();

	while(item != modules_vector.rend())
	{
		delete *item;
		++item ;
	}
}

bool GnEngine::Init()
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
void GnEngine::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void GnEngine::FinishUpdate()
{

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status GnEngine::Update()
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

bool GnEngine::CleanUp()
{
	bool ret = true;
	for (int i = modules_vector.size() -1; i > 0; i--)
	{
		modules_vector[i]->CleanUp();
	}

	return ret;
}

void GnEngine::AddModule(Module* mod)
{
	modules_vector.push_back(mod);
}