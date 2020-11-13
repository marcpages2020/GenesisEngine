#pragma once
#include "Globals.h"
#include "Module.h"

#include <map>

class Resource;
enum ResourceType;

class ModuleResources : public Module 
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	void ImportFile(const char* file_path, bool drag_and_drop = false);
	ResourceType GetResourceTypeFromPath(const char* path);

private:
	std::map<uint, Resource*> resources;
};