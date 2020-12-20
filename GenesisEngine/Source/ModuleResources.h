#pragma once
#include "Globals.h"
#include "Module.h"
#include "Resource.h"
#include "ImportingOptions.h"

#include <map>
#include <string>

class GameObject;
struct AssetsIcons;

struct ResourceData
{
	std::string name;
	std::string assetsFile;
	std::string libraryFile;
	ResourceType type = ResourceType::RESOURCE_UNKNOWN;
};

class ModuleResources : public Module 
{
public:
	ModuleResources(bool start_enabled = true);
	~ModuleResources();

	bool Init() override;
	bool CleanUp() override;
	void OnEditor();
	void LoadEngineAssets(AssetsIcons& icons);
	void OnFrameEnd() override;

	bool MetaUpToDate(const char* assets_file, const char* meta_file);
	int GetUIDFromMeta(const char* meta_file);
	int Find(const char* assets_file);
	const char* Find(uint UID);
	std::string FindAsset(uint UID);
	const char* GetLibraryPath(uint UID);
	bool Exists(uint UID);

	uint ImportFile(const char* assets_file);
	uint ImportInternalResource(const char* path, const void* data, ResourceType type, uint UID = 0);
	uint ReimportFile(const char* assets_file);
	void CreateResourceData(uint UID, const char* name = "No Name", const char* assets_path = "No path", const char* library_path = "No path");
	void DragDropFile(const char* path);

	void AddAssetToDelete(const char* asset_path);
	void AddResourceToDelete(uint UID);
	bool DeleteAsset(const char* assets_path);
	bool DeleteResource(uint UID);
	bool DeleteInternalResource(uint UID);
	bool DeleteInternalResources(uint UID);

	Resource* LoadResource(uint UID, ResourceType type);
	void UnloadResource(Resource* resource);

	Resource* CreateResource(const char* assetsPath, ResourceType type, uint UID = 0);
	Resource* CreateResource(uint UID, ResourceType type, std::string assets_file = "");
	Resource* RequestResource(uint UID);
	ResourceData RequestResourceData(uint UID);
	GameObject* RequestGameObject(const char* assets_file);
	void ReleaseResource(uint UID);
	void ReleaseResourceData(uint UID);
	bool SaveResource(Resource* resource);
	bool SaveMetaFile(Resource* resource);
	bool LoadMetaFile(Resource* resource);

	ResourceType GetTypeFromPath(const char* path);
	uint GenerateUID();
	const char* GenerateLibraryPath(Resource* resource);
	std::string GenerateLibraryPath(uint uid, ResourceType type);
	std::string GetLibraryFolder(const char* file_in_assets);
	std::string GenerateAssetsPath(const char* path);
	std::string GenerateMetaFile(const char* assets_path);
	void AddFileExtension(std::string& file, ResourceType type);

	void CheckAssetsRecursive(const char* from_directory);
	void CleanLibrary();

public:
	ModelImportingOptions modelImportingOptions;
	TextureImportingOptions textureImportingOptions;
	bool cleanLibrary;
	bool cleanMetas;
private:
	std::map<uint, Resource*> resources;
	std::map<uint, ResourceData> resources_data;
	std::string _toDeleteAsset;
	uint _toDeleteResource;
};