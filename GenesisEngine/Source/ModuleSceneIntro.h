#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	bool Init();
	bool LoadConfig(JSON_Object* config) override;
	update_status Update(float dt);
	bool CleanUp();

public:
	bool show_grid;
};
