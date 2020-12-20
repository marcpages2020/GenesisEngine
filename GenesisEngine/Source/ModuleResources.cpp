#include "ModuleResources.h"
#include "GameObject.h"
#include "FileSystem.h"
#include "Importers.h"
#include "ModuleScene.h"
#include "Application.h"

#include "GnJSON.h"

#include "Resource.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"

#include "WindowImport.h"
#include "WindowAssets.h"
#include <algorithm>

#include "MathGeoLib/include/MathGeoLib.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled), _toDeleteResource(0u), cleanLibrary(false), cleanMetas(false)
{
	name = "resources";
	modelImportingOptions = ModelImportingOptions();
	textureImportingOptions = TextureImportingOptions();
	_toDeleteAsset.clear();
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init()
{
	bool ret = true;

	MeshImporter::Init();
	TextureImporter::Init();

	App->renderer3D->Init();

	//std::vector<std::string> files;
	//std::vector<std::string> dirs;
	//FileSystem::DiscoverFilesRecursive("Library", files, dirs);

	CheckAssetsRecursive("Assets");

	return ret;
}

bool ModuleResources::CleanUp()
{
	bool ret = true;
	std::map<uint, Resource*>::iterator it = resources.begin();

	/*
	for (it; it != resources.end(); ++it) 
	{
		ReleaseResource(it->second->GetUID());
	}
	*/

	resources.clear();
	resources_data.clear();

	if (cleanLibrary)
	{
		CleanLibrary();
	}

	return ret;
}

void ModuleResources::OnEditor()
{
	std::vector<Resource*> meshes;
	std::vector<Resource*> materials;
	std::vector<Resource*> textures;

	std::map<uint, Resource*>::iterator it = resources.begin();
	for (it; it != resources.end(); it++)
	{
		switch (it->second->GetType())
		{
		case ResourceType::RESOURCE_MESH:
			meshes.push_back(it->second);
			break;
		case ResourceType::RESOURCE_MATERIAL:
			materials.push_back(it->second);
			break;
		case ResourceType::RESOURCE_TEXTURE:
			textures.push_back(it->second);
			break;
		default:
			break;
		}
	}

	if(ImGui::TreeNode("Meshes")) {
		ImGui::Separator();
		for (size_t i = 0; i < meshes.size(); i++)
		{
			ImGui::Text("Name: %s", meshes[i]->name.c_str());
			ImGui::Text("UID: %d", meshes[i]->GetUID());
			ImGui::Text("Assets path: %s", meshes[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", meshes[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", meshes[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Materials")) {
		for (size_t i = 0; i < materials.size(); i++)
		{
			ImGui::Text("Name: %s", materials[i]->name.c_str());
			ImGui::Text("UID: %d", materials[i]->GetUID());
			ImGui::Text("Assets path: %s", materials[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", materials[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", materials[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Textures")) {
		for (size_t i = 0; i < textures.size(); i++)
		{
			ImGui::Text("Name: %s", textures[i]->name.c_str());
			ImGui::Text("UID: %d", textures[i]->GetUID());
			ImGui::Text("Assets path: %s", textures[i]->assetsFile.c_str());
			ImGui::Text("Library path: %s", textures[i]->libraryFile.c_str());
			ImGui::Text("Reference count: %d", textures[i]->referenceCount);
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}
		ImGui::TreePop();
	}
}

void ModuleResources::LoadEngineAssets(AssetsIcons& icons)
{
	icons.folder = dynamic_cast<ResourceTexture*>(RequestResource(Find("Assets/EngineAssets/folder.png")));
	icons.model = dynamic_cast<ResourceTexture*>(RequestResource(Find("Assets/EngineAssets/model.png")));
}

void ModuleResources::OnFrameEnd()
{
	if(_toDeleteResource != 0u)
	{
		DeleteResource(_toDeleteResource);
		_toDeleteResource = 0u;
	}

	if (_toDeleteAsset.size() > 0)
	{
		DeleteAsset(_toDeleteAsset.c_str());
		_toDeleteAsset.clear();
	}
}

bool ModuleResources::MetaUpToDate(const char* assets_file, const char* meta_file)
{
	bool ret = true;

	char* buffer = nullptr;
	uint size = FileSystem::Load(meta_file, &buffer);
	GnJSONObj meta(buffer);

	uint UID = meta.GetInt("UID");
	int lastModifiedMeta = meta.GetInt("lastModified");
	uint lastModified = FileSystem::GetLastModTime(assets_file);

	if (lastModifiedMeta == lastModified && UID != 0)
	{
		std::string library_path = meta.GetString("Library path", "Library/NoPath");

		if (library_path == "Library/NoPath")
			library_path = Find(UID);

		//check for the file itself to exist
		if (!FileSystem::Exists(library_path.c_str())) 
		{
			ret = false;
		}
		//check its internal resources
		else if (GetTypeFromPath(assets_file) == ResourceType::RESOURCE_MODEL) 
		{
			ret = ModelImporter::InternalResourcesExist(meta_file);
		}

		resources_data[UID].assetsFile = assets_file;
		resources_data[UID].libraryFile = library_path;
		resources_data[UID].type = GetTypeFromPath(assets_file);
	}
	else
	{
		ret = false;
	}

	meta.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

int ModuleResources::GetUIDFromMeta(const char* meta_file)
{
	if (!FileSystem::Exists(meta_file))
		return 0;

	char* buffer = nullptr;
	uint size = FileSystem::Load(meta_file, &buffer);
	GnJSONObj meta(buffer);

	int UID = meta.GetInt("UID", -1);

	meta.Release();
	RELEASE_ARRAY(buffer);

	return UID;
}

int ModuleResources::Find(const char* assets_file)
{
	int UID = 0;

	std::map<uint, Resource*>::iterator resource_it = resources.begin();
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.begin();

	//First we loop through all loaded resources
	for (resource_it; resource_it != resources.end(); resource_it++) {
		if (FileSystem::ToLower(resource_it->second->assetsFile.c_str()) == FileSystem::ToLower(assets_file))
			return resource_it->first;
	}

	//If not found we loop through all not loaded but known resources
	for (resources_data_it; resources_data_it != resources_data.end(); resources_data_it++) {
		if (FileSystem::ToLower(resources_data_it->second.assetsFile.c_str()) == FileSystem::ToLower(assets_file))
			return resources_data_it->first;
	}

	return UID;
}

const char* ModuleResources::Find(uint UID)
{
	if (resources_data.find(UID) != resources_data.end() && resources_data[UID].libraryFile.size() > 0) 
		return resources_data[UID].libraryFile.c_str();

	std::vector<std::string> directories = { "Library/Config/","Library/Models/","Library/Meshes/","Library/Materials/","Library/Textures/", "Library/Scenes/" };
	std::vector<std::string> extensions = { ".json",".model",".mesh",".material",".dds", ".scene" };

	for (size_t i = 0; i < directories.size(); i++)
	{
		std::string file = directories[i];
		file += std::to_string(UID);
		file += extensions[i];
		if (FileSystem::Exists(file.c_str()))
		{
			char* final_file = new char[256];
			sprintf_s(final_file, 256, file.c_str());
			std::string library_path = final_file;
			resources_data[UID].libraryFile = library_path;
			return final_file;
		}
	}

	return nullptr;
}

std::string ModuleResources::FindAsset(uint UID)
{
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.find(UID);

	if (resources_data_it != resources_data.end()) {
		return resources_data_it->second.assetsFile;
	}
}

const char* ModuleResources::GetLibraryPath(uint UID)
{
	std::map<uint, Resource*>::iterator resources_it = resources.find(UID);
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.find(UID);

	if (resources_it != resources.end()) {
		return resources_it->second->libraryFile.c_str();
	}

	if (resources_data_it != resources_data.end()) {
		return resources_data_it->second.libraryFile.c_str();
	}
}

bool ModuleResources::Exists(uint UID)
{
	std::map<uint, Resource*>::iterator resource = resources.find(UID);
	std::map<uint, ResourceData>::iterator resource_data = resources_data.find(UID);

	return resource != resources.end() || resource_data != resources_data.end();
}



uint ModuleResources::ImportFile(const char* assets_file)
{
	ResourceType type = GetTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;
	
	char* fileBuffer;

	uint size = FileSystem::Load(assets_file, &fileBuffer);

	std::string library_path;

	switch (type)
	{
	case RESOURCE_MODEL: 
		ModelImporter::Import(fileBuffer, (ResourceModel*)resource, size); 
		break;
	case RESOURCE_TEXTURE:
		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size);
		break;
	case RESOURCE_SHADER:
		ShaderImporter::Import(fileBuffer, (ResourceShader*)resource, assets_file);
		break;
	case RESOURCE_SCENE: 
		library_path = "Library/Scenes/";
		library_path.append(FileSystem::GetFile(assets_file));
		FileSystem::DuplicateFile(assets_file, library_path.c_str());
		library_path.clear();
		break;
	default: 
		LOG_WARNING("Trying to import unknown file: %s", assets_file);
		break;
	}

	if (resource == nullptr) 
	{
		if(type != ResourceType::RESOURCE_SCENE)
			LOG_ERROR("Fatal error when importing file: %s", assets_file);

		return 0;
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

uint ModuleResources::ImportInternalResource(const char* path, const void* data, ResourceType type, uint UID)
{
	uint ret = 0;
	Resource* resource = CreateResource(path, type, UID);

	switch (type)
	{
	case RESOURCE_MESH:
		MeshImporter::Import((aiMesh*)data, (ResourceMesh*)resource);
		break;
	case RESOURCE_MATERIAL:
		MaterialImporter::Import((aiMaterial*)data, (ResourceMaterial*)resource);
		break;
	default:
		break;
	}
		
	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);

	return ret;
}

uint ModuleResources::ReimportFile(const char* assets_file)
{
	LOG_WARNING("Reimporting file: %s", assets_file);

	std::string meta_file = assets_file;
	meta_file.append(".meta");

	uint ret = GetUIDFromMeta(meta_file.c_str());
	ResourceType type = GetTypeFromPath(assets_file);
	Resource* resource = CreateResource(assets_file, type, ret);

	char* fileBuffer;
	uint size = FileSystem::Load(assets_file, &fileBuffer);

	if (size <= 0)
	{
		LOG_ERROR("Trying to load unexisting file: %s", assets_file);
		return 0;
	}

	switch (type)
	{
	case ResourceType::RESOURCE_MODEL:
		ModelImporter::ReimportFile(fileBuffer, (ResourceModel*)resource, size);
		break;
	case ResourceType::RESOURCE_TEXTURE:
		if (FileSystem::Exists(resource->libraryFile.c_str()))
			FileSystem::Delete(resource->libraryFile.c_str());

		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size);
		break;
	default:
		break;
	}

	if (resource == nullptr) 
	{
		LOG_ERROR("Fatal error when reimporting file: %s", assets_file);
		return 0;
	}
	else
	{
		LOG("File: %s reimported successfully", assets_file);
	}

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

void ModuleResources::CreateResourceData(uint UID, const char* name, const char* assets_path, const char* library_path)
{
	resources_data[UID].name = name;
	resources_data[UID].assetsFile = assets_path;
	resources_data[UID].libraryFile = library_path;
	resources_data[UID].type = GetTypeFromPath(library_path);
}

void ModuleResources::DragDropFile(const char* path)
{
	WindowImport* import_window = dynamic_cast<WindowImport*>(App->editor->windows[WindowType::IMPORT_WINDOW]);
	import_window->Enable(path, GetTypeFromPath(path));
}

void ModuleResources::AddAssetToDelete(const char* asset_path)
{
	std::string meta_file = asset_path;
	meta_file.append(".meta");
	_toDeleteResource = GetUIDFromMeta(meta_file.c_str());
	_toDeleteAsset = asset_path;
	App->AddModuleToTaskStack(this);
}

void ModuleResources::AddResourceToDelete(uint UID)
{
	_toDeleteResource = UID;
	App->AddModuleToTaskStack(this);
}

bool ModuleResources::DeleteAsset(const char* assets_path)
{
	bool ret = true;

	FileSystem::Delete(assets_path);

	std::string meta_file = assets_path;
	meta_file.append(".meta");

	if(FileSystem::Exists(meta_file.c_str()))
		FileSystem::Delete(meta_file.c_str());

	return ret;
}

bool ModuleResources::DeleteResource(uint UID)
{
	bool ret = true;

	switch (resources_data[UID].type)
	{
	case ResourceType::RESOURCE_MODEL:
		DeleteInternalResources(UID);
		break;
	default:
		break;
	}

	if (resources.find(UID) != resources.end())
		UnloadResource(resources[UID]);

	FileSystem::Delete(resources_data[UID].libraryFile.c_str());
	ReleaseResourceData(UID);

	return ret;
}

bool ModuleResources::DeleteInternalResources(uint UID)
{
	bool ret = true;
	std::string meta_file = resources_data[UID].assetsFile + ".meta";

	if (resources_data[UID].type == ResourceType::RESOURCE_MODEL)
	{
		std::vector<uint> meshes;
		std::vector<uint> materials;
		ModelImporter::ExtractInternalResources(resources_data[UID].libraryFile.c_str(), meshes, materials);

		for (size_t i = 0; i < meshes.size(); i++) 
		{
			DeleteInternalResource(meshes[i]);
		}

		for (size_t i = 0; i < materials.size(); i++) 
		{
			DeleteInternalResource(materials[i]);
		}
	}

	return ret;
}

bool ModuleResources::DeleteInternalResource(uint UID)
{
	bool ret = true;

	ret = FileSystem::Delete(resources_data[UID].libraryFile.c_str());

	if (resources.find(UID) != resources.end())
		UnloadResource(resources[UID]);

	resources_data.erase(resources_data.find(UID));

	return ret;
}

Resource* ModuleResources::LoadResource(uint UID, ResourceType type)
{
	Resource* resource = CreateResource(UID, type, resources_data[UID].assetsFile.c_str());
	bool ret = true;

	char* buffer = nullptr;
	uint size = FileSystem::Load(resource->libraryFile.c_str(), &buffer);

	if (size >= 0)
	{
		switch (resource->GetType())
		{
		case RESOURCE_MODEL:
			if(ModelImporter::InternalResourcesExist(resource->libraryFile.c_str()))
				ret = ModelImporter::Load(buffer, (ResourceModel*)resource, size);
			else {
				ReimportFile(resources_data[UID].assetsFile.c_str());
				ret = ModelImporter::Load(buffer, (ResourceModel*)resource, size);
			}
			break;
		case RESOURCE_MESH:
			ret = MeshImporter::Load(buffer, (ResourceMesh*)resource, size);
			break;
		case RESOURCE_MATERIAL:
			ret = MaterialImporter::Load(buffer, (ResourceMaterial*)resource, size);
			break;
		case RESOURCE_TEXTURE:
			ret = TextureImporter::Load(buffer, (ResourceTexture*)resource, size);
			LoadMetaFile(resource);
			break;
		case RESOURCE_SCENE:
			break;
		case RESOURCE_UNKNOWN:
			break;
		default:
			break;
		}
	}
	else
		ret = false;
	
	if(ret == false)
	{
		LOG_ERROR("Resource: %d could not be loaded");
		ReleaseResource(UID);
		resource = nullptr;
		return nullptr;
	}

	resource->name = resources_data[UID].name;
	RELEASE_ARRAY(buffer);

	return resource;
}

void ModuleResources::UnloadResource(Resource* resource)
{
	switch (resource->GetType())
	{
	case ResourceType::RESOURCE_MODEL:
		break;
	case ResourceType::RESOURCE_MESH:
		break;
	case ResourceType::RESOURCE_MATERIAL:
		break;
	case ResourceType::RESOURCE_TEXTURE:
		TextureImporter::UnloadTexture(((ResourceTexture*)resource)->GetID());
		break;
	default:
		break;
	}

	resources.erase(resources.find(resource->GetUID()));

	delete resource;
	resource = nullptr;
}

Resource* ModuleResources::CreateResource(const char* assetsPath, ResourceType type, uint UID)
{
	Resource* resource = nullptr;

	if(UID == 0)
		UID = GenerateUID();

	std::string extension = FileSystem::GetFileExtension(assetsPath);

	switch (type)
	{
	case RESOURCE_MODEL:
		resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		resource = new ResourceTexture(UID);
		break;
	case RESOURCE_SHADER:
		resource = new ResourceShader(UID);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		resources[UID] = resource;
		resources[UID]->name = FileSystem::GetFileName(assetsPath);
		resource->assetsFile = FileSystem::ToLower(assetsPath);
		resource->libraryFile = GenerateLibraryPath(resource);

		resources_data[UID].name = resources[UID]->name;
		resources_data[UID].assetsFile = resource->assetsFile;
		resources_data[UID].libraryFile = resource->libraryFile;
		resources_data[UID].type = type;
	}

	return resource;
}

Resource* ModuleResources::CreateResource(uint UID, ResourceType type, std::string assets_file)
{
	Resource* resource = nullptr;

	switch (type)
	{
	case RESOURCE_MODEL:
		resource = new ResourceModel(UID);
		break;
	case RESOURCE_MESH:
		resource = new ResourceMesh(UID);
		break;
	case RESOURCE_MATERIAL:
		resource = new ResourceMaterial(UID);
		break;
	case RESOURCE_TEXTURE:
		resource = new ResourceTexture(UID);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		if (assets_file.size() > 0)
			resource->assetsFile = assets_file;
		else
			resource->assetsFile = resources_data[UID].assetsFile;

		resource->libraryFile = GenerateLibraryPath(resource);
		resource->name = FileSystem::GetFileName(resource->assetsFile.c_str());

		resources[UID] = resource;
	}

	return resource;
}

Resource* ModuleResources::RequestResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end() && it->second != nullptr) {
		it->second->referenceCount++;
		return it->second;
	}

	const char* library_file = Find(UID);

	if (library_file == nullptr)
		return nullptr;

	ResourceType type = GetTypeFromPath(library_file);
	Resource* resource = LoadResource(UID, type);
	
	if (resource != nullptr)
		resource->referenceCount++;

	return resource;
}

ResourceData ModuleResources::RequestResourceData(uint UID)
{
	std::map<uint, ResourceData>::iterator it = resources_data.find(UID);

	if (it != resources_data.end()) 
		return it->second;
}

GameObject* ModuleResources::RequestGameObject(const char* assets_file)
{
	if (App->resources->GetTypeFromPath(assets_file) != ResourceType::RESOURCE_MODEL)
		return nullptr;

	std::string meta_file = assets_file;
	meta_file.append(".meta");
	ResourceModel* model = (ResourceModel*)RequestResource(GetUIDFromMeta(meta_file.c_str()));

	if (model == nullptr) 
	{	
		model = (ResourceModel*)RequestResource(ReimportFile(assets_file));
	}

	return ModelImporter::ConvertToGameObject(model);
}

void ModuleResources::ReleaseResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		it->second->referenceCount--;

		if (it->second->referenceCount <= 0)
			UnloadResource(it->second);
	}
}

void ModuleResources::ReleaseResourceData(uint UID)
{
	resources_data.erase(resources_data.find(UID));
}

bool ModuleResources::SaveResource(Resource* resource)
{
	bool ret = true;

	char* buffer;
	uint size = 0;

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		size = ModelImporter::Save((ResourceModel*)resource, &buffer);
		break;
	case RESOURCE_MESH:
		size = MeshImporter::Save((ResourceMesh*)resource, &buffer);
		break;
	case RESOURCE_MATERIAL:
		size = MaterialImporter::Save((ResourceMaterial*)resource, &buffer);
		break;
	case RESOURCE_TEXTURE:
		size = TextureImporter::Save((ResourceTexture*)resource, &buffer);
		break;
	case RESOURCE_SCENE:
		break;
	default:
		break;
	}

	if (size > 0) 
	{
		FileSystem::Save(resource->libraryFile.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
	}

	if(resource->GetType() == ResourceType::RESOURCE_MODEL || resource->GetType() == ResourceType::RESOURCE_TEXTURE)
		ret = SaveMetaFile(resource);

	return ret;
}

bool ModuleResources::SaveMetaFile(Resource* resource)
{
	GnJSONObj base_object;
	resource->SaveMeta(base_object, FileSystem::GetLastModTime(resource->assetsFile.c_str()));

	char* meta_buffer = NULL;
	uint meta_size = base_object.Save(&meta_buffer);

	std::string meta_file_name = resource->assetsFile + ".meta";
	FileSystem::Save(meta_file_name.c_str(), meta_buffer, meta_size);

	base_object.Release();
	RELEASE_ARRAY(meta_buffer);

	return true;
}

bool ModuleResources::LoadMetaFile(Resource* resource)
{
	bool ret = true;

	std::string meta_file = resource->assetsFile;
	meta_file.append(".meta");

	char* buffer = nullptr;
	FileSystem::Load(meta_file.c_str(), &buffer);

	GnJSONObj meta_data(buffer);

	resource->Load(meta_data);

	meta_data.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

ResourceType ModuleResources::GetTypeFromPath(const char* path)
{
	std::string extension = FileSystem::GetFileExtension(path);
	
	if (extension == ".fbx" || extension == ".model")
		return ResourceType::RESOURCE_MODEL;

	else if (extension == ".mesh")
		return ResourceType::RESOURCE_MESH;

	else if (extension == ".material")
		return ResourceType::RESOURCE_MATERIAL;

	else if (extension == ".png" || extension == ".tga" || extension == ".dds")
		return ResourceType::RESOURCE_TEXTURE;

	else if (extension == ".vert" || extension == ".frag")
		return ResourceType::RESOURCE_SHADER;

	else if (extension == ".scene")
		return ResourceType::RESOURCE_SCENE;

	else 
		return ResourceType::RESOURCE_UNKNOWN;
}

uint ModuleResources::GenerateUID()
{
	return LCG().Int();
}

const char* ModuleResources::GenerateLibraryPath(Resource* resource)
{
	char* library_path = new char[128];

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		sprintf_s(library_path, 128, "Library/Models/%d.model", resource->GetUID()); break;
	case RESOURCE_MESH:
		sprintf_s(library_path, 128, "Library/Meshes/%d.mesh", resource->GetUID()); break;
	case RESOURCE_MATERIAL:
		sprintf_s(library_path, 128, "Library/Materials/%d.material", resource->GetUID()); break;
	case RESOURCE_TEXTURE:
		sprintf_s(library_path, 128, "Library/Textures/%d.dds", resource->GetUID()); break;
	case RESOURCE_SCENE:
		sprintf_s(library_path, 128, "Library/Scenes/%d.scene", resource->GetUID()); break;
	default:
		break;
	}

	return library_path;
}

std::string ModuleResources::GenerateLibraryPath(uint uid, ResourceType type)
{
	std::string path;
	switch (type)
	{
	case RESOURCE_MODEL:
		path = "Library/Models/";
		path.append(std::to_string(uid) + ".model");
		break;
	case RESOURCE_MESH:
		path = "Library/Meshes/";
		path.append(std::to_string(uid) + ".mesh");
		break;
	case RESOURCE_MATERIAL:
		path = "Library/Materials/";
		path.append(std::to_string(uid) + ".material");
		break;
	case RESOURCE_TEXTURE:
		path = "Library/Textures/";
		path.append(std::to_string(uid) + ".dds");
		break;
	case RESOURCE_SCENE:
		path = "Library/Scenes/";
		path.append(std::to_string(uid) + ".scene");
		break;
	case RESOURCE_UNKNOWN:
		LOG("Error trying to generate a path for an unknown file %d", uid);
		break;
	default:
		break;
	}

	return path;
}

std::string ModuleResources::GetLibraryFolder(const char* file_in_assets)
{
	ResourceType type = GetTypeFromPath(file_in_assets);

	switch (type)
	{
	case RESOURCE_MODEL: return std::string("Library/Models/"); break;
	case RESOURCE_MESH: return std::string("Library/Meshes/"); break;
	case RESOURCE_MATERIAL: return std::string("Library/Materials/"); break;
	case RESOURCE_TEXTURE: return std::string("Library/Textures/");	break;
	case RESOURCE_SCENE: return std::string("Library/Scenes/");	break;
	default: break;
	}
}

std::string ModuleResources::GenerateAssetsPath(const char* path)
{
	ResourceType type = GetTypeFromPath(path);
	std::string file = FileSystem::NormalizePath(path);
	file = FileSystem::GetFile(file.c_str());

	std::string assets_path;

	switch (type)
	{
	case RESOURCE_MODEL:
		assets_path = "Assets/Models/"; break;
	case RESOURCE_TEXTURE:
		assets_path = "Assets/Textures/"; break;
	case RESOURCE_SCENE:
		assets_path = "Assets/Scenes/"; break;
	default:
		break;
	}

	assets_path.append(file);

	return assets_path;
}

std::string ModuleResources::GenerateMetaFile(const char* assets_path)
{
	std::string meta_file = assets_path;
	meta_file.append(".meta");
	return meta_file;
}

void ModuleResources::AddFileExtension(std::string& file, ResourceType type)
{
	switch (type)
	{
	case RESOURCE_MODEL: file += ".model"; break;
	case RESOURCE_MESH: file += ".mesh"; break;
	case RESOURCE_MATERIAL: file += ".material"; break;
	case RESOURCE_TEXTURE: file += ".dds"; break;
	case RESOURCE_SCENE:  file += ".scene";	break;
	default: break;
	}
}

void ModuleResources::CheckAssetsRecursive(const char* directory)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	FileSystem::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		CheckAssetsRecursive((dir + (*it)).c_str());
	}

	std::sort(files.begin(), files.end());

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string file = directory;
		file.append("/" + files[i]);

		//ignore metas and jsons
		if ((file.find(".meta") != std::string::npos) || (file.find(".json") != std::string::npos))
			continue;

		std::string meta = file;
		meta.append(".meta");

		if (FileSystem::Exists(meta.c_str()))
		{
			if (!MetaUpToDate(file.c_str(), meta.c_str()))
			{
				ReimportFile(file.c_str());
			}
		}
		else
		{
			ImportFile(file.c_str());
		}
	}
}

void ModuleResources::CleanLibrary()
{
	std::vector<std::string> folders;
	std::vector<std::string> files;

	FileSystem::DiscoverFilesRecursive("Library", files, folders);

	for (size_t i = 0; i < files.size(); i++)
	{
		//avoid deleting config files
		if(files[i].find("Config") == std::string::npos)
			FileSystem::Delete(files[i].c_str());
	}

	folders.clear();
	files.clear();

	FileSystem::DiscoverFilesRecursive("Assets", files, folders);

	if (cleanMetas)
	{
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i].find(".meta") != std::string::npos)
			{
				FileSystem::Delete(files[i].c_str());
			}
		}
	}
	
	folders.clear();
	files.clear();
}
