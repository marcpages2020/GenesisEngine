#pragma once
#include "Globals.h"
#include "Module.h"
#include "Resource.h"

#include <map>
#include <string>

struct ResourceData
{
	ResourceType type = ResourceType::RESOURCE_UNKNOWN;
	std::string assetsFile;
	std::string libraryFile;
};

class ModuleResources : public Module 
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init() override;

	int MetaUpToDate(const char* asset_path);
	uint Find(const char* assets_file);

	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type);
	void CreateResourceData(uint UID, ResourceType type, const char* assets_path = "No path");

	Resource* LoadResource(uint UID);

	Resource* CreateResource(const char* assetsPath, ResourceType type);
	Resource* CreateResource(uint UID);
	Resource* RequestResource(uint UID);
	void ReleaseResource(uint UID);
	bool SaveResource(Resource* resource);
	bool SaveMetaFile(Resource* resource);

	ResourceType GetResourceTypeFromPath(const char* path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);
	std::string GetLibraryFolder(const char* file_in_assets);
	void AddFileExtension(std::string& file, ResourceType type);

private:
	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resources_data;
};