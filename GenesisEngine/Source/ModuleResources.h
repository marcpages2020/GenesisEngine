#pragma once
#include "Module.h"
class ModuleResources : public Module
{
public:
	ModuleResources(GnEngine* app, bool start_enabled = true);

	bool Start();
};

