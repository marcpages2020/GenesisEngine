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

	bool MetaUpToDate(const char* asset_path);
	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type);

	Resource* CreateResource(const char* assetsPath, ResourceType type);
	Resource* RequestResource(uint UID);
	void ReleaseResource(uint UID);
	bool SaveResource(Resource* resource);
	bool SaveMetaFile(Resource* resource);

	ResourceType GetResourceTypeFromPath(const char* path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);


private:
	std::map<uint, Resource*> resources;
};