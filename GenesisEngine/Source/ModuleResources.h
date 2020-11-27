#pragma once
#include "Globals.h"
#include "Module.h"
#include "Resource.h"

#include <map>
#include <string>

class GameObject;

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
	void OnEditor();

	int MetaUpToDate(const char* asset_path);
	int UpdateMetaFile(GnJSONObj& meta_file);
	int Find(const char* assets_file);
	const char* Find(uint UID);
	const char* GetLibraryPath(uint UID);

	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type);
	void CreateResourceData(uint UID, const char* assets_path = "No path", const char* library_path = "No path");

	bool DeleteAssetsResource(const char* assets_path);
	bool DeleteResource(uint UUID);
	bool DeleteInternalResource(uint UID);

	Resource* LoadResource(uint UID, ResourceType type);
	void UnloadResource(uint UID);

	Resource* CreateResource(const char* assetsPath, ResourceType type);
	Resource* CreateResource(uint UID, ResourceType type);
	Resource* RequestResource(uint UID);
	GameObject* RequestGameObject(const char* assets_file);
	void ReleaseResource(uint UID);
	void ReleaseResourceData(uint UID);
	bool SaveResource(Resource* resource);
	bool SaveMetaFile(Resource* resource);

	ResourceType GetResourceTypeFromPath(const char* library_path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);
	std::string GetLibraryFolder(const char* file_in_assets);
	const char* GenerateAssetsPath(const char* path);
	void AddFileExtension(std::string& file, ResourceType type);

private:
	void CheckAssetsRecursive(const char* directory);

private:
	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resources_data;
};