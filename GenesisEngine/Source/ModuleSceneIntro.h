#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	bool Init(JSON_Object* object);
	update_status Update(float dt);
	bool CleanUp();

	void PracticeOpenGL();

public:
	bool show_grid;
};
