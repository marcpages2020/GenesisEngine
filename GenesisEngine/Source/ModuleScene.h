#pragma once
#include "Module.h"

class ResourceModel;

class ModuleScene : public Module
{
public:
	ModuleScene(GnEngine* app, bool start_enabled = true);

	bool Start();
	update_status Update(float deltaTime);
	bool CleanUp();

private:
	ResourceModel* resourceModel;
};


