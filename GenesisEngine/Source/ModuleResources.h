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

	bool Init() override;

	uint ImportFile(const char* assets_file, bool drag_and_drop = false);
	Resource* CreateResource(const char* assetsPath, ResourceType type);
	bool SaveResource(Resource* resource);

	ResourceType GetResourceTypeFromPath(const char* path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);


private:
	std::map<uint, Resource*> resources;
};