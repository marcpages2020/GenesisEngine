#pragma once
#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include "ImportingOptions.h"

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
	update_status PostUpdate(float dt) override;
	void OnEditor();
	void LoadEngineAssets();
	void OnFrameEnd() override;

	bool MetaUpToDate(const char* assets_file, const char* meta_file);
	bool UpdateAssetsResource(const char* assets_path);
	uint GetUIDFromMeta(const char* meta_file);
	int Find(const char* assets_file);
	const char* Find(uint UID);
	const char* GetLibraryPath(uint UID);
	bool Exists(uint UID);

	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type);
	void CreateResourceData(uint UID, const char* assets_path = "No path", const char* library_path = "No path");
	void DragDropFile(const char* path);
	void DrawImportingWindow();

	void AddResourceToDelete(uint UID);
	bool DeleteAssetsResource(const char* assets_path);
	bool DeleteResource(uint UID);
	bool DeleteInternalResources(uint UID);
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

	ResourceType GetTypeFromPath(const char* path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);
	std::string GetLibraryFolder(const char* file_in_assets);
	const char* GenerateAssetsPath(const char* path);
	void AddFileExtension(std::string& file, ResourceType type);

public:
	ModelImportingOptions modelImportingOptions;
	TextureImportingOptions textureImportingOptions;

private:
	void CheckAssetsRecursive(const char* directory);

private:
	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resources_data;
	int _toDeleteResource;
	bool _choosingImportingOptions;

	char* currentImportingFile;
	ResourceType currentImportingFileType;
};