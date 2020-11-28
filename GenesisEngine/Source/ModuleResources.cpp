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

#include <algorithm>

#include "MathGeoLib/include/MathGeoLib.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled), _toDeleteResource(-1)
{
	name = "resources";
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init()
{
	bool ret = true;

	MeshImporter::Init();
	TextureImporter::Init();

	CheckAssetsRecursive("Assets");
	//LoadEngineAssets();

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

void ModuleResources::LoadEngineAssets()
{
	ResourceTexture* folder_tex = dynamic_cast<ResourceTexture*>(RequestResource(Find("Assets/EngineAssets/folder.png")));
}

void ModuleResources::OnFrameEnd()
{
	if(_toDeleteResource != -1)
	{
		DeleteResource(_toDeleteResource);
	}
}

bool ModuleResources::MetaUpToDate(const char* assets_file, const char* meta_file)
{
	bool ret = false;

	char* buffer = nullptr;
	uint size = FileSystem::Load(meta_file, &buffer);
	GnJSONObj meta(buffer);

	uint UID = meta.GetInt("UID");
	int lastModifiedMeta = meta.GetInt("lastModified");
	uint lastModified = FileSystem::GetLastModTime(assets_file);

	if (lastModifiedMeta == lastModified)
	{
		std::string library_path = meta.GetString("Library path", "NoPath");

		resources_data[UID].assetsFile = assets_file;
		resources_data[UID].type = GetTypeFromPath(assets_file);

		if (library_path == "NoPath")
			resources_data[UID].libraryFile = Find(UID);
		else
			resources_data[UID].libraryFile = library_path;

		resources_data[UID].type = GetTypeFromPath(library_path.c_str());

		ret = true;
	}

	meta.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

bool ModuleResources::UpdateAssetsResource(const char* assets_path)
{
	//char* buffer = nullptr;
	//std::string meta_file = assets_file;
	//meta_file.append(".meta");

	//uint size = FileSystem::Load(meta_file.c_str(), &buffer);
	//GnJSONObj meta(buffer);

	//uint UID = meta.GetInt("UID");
	//int lastModifiedMeta = meta.GetInt("lastModified");
	//uint lastModified = FileSystem::GetLastModTime(asset_path);

	//Find(UID);
	return 0;
}

uint ModuleResources::GetUIDFromMeta(const char* meta_file)
{
	char* buffer = nullptr;
	uint size = FileSystem::Load(meta_file, &buffer);
	GnJSONObj meta(buffer);

	int UID = meta.GetInt("UID");

	meta.Release();
	RELEASE_ARRAY(buffer);

	return UID;
}

int ModuleResources::Find(const char* assets_file)
{
	int UID = -1;

	std::map<uint, Resource*>::iterator resource_it = resources.begin();
	std::map<uint, ResourceData>::iterator resources_data_it = resources_data.begin();

	//First we loop through all loaded resources
	for (resource_it; resource_it != resources.end(); resource_it++) {
		if (resource_it->second->assetsFile == assets_file)
			return resource_it->first;
	}

	//If not found we loop through all not loaded but known resources
	for (resources_data_it; resources_data_it != resources_data.end(); resources_data_it++) {
		if (resources_data_it->second.assetsFile == assets_file)
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
			return final_file;
		}
	}

	return nullptr;
}

const char* ModuleResources::GetLibraryPath(uint UID)
{
	return resources[UID]->libraryFile.c_str();
}

bool ModuleResources::Exists(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end())
		return true;
	else
		return false;
}



uint ModuleResources::ImportFile(const char* assets_file)
{
	std::string meta_file = assets_file;
	meta_file.append(".meta");

	if(FileSystem::Exists(meta_file.c_str()))
		return GetUIDFromMeta(meta_file.c_str());

	ResourceType type = GetTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;
	
	char* fileBuffer;
	uint size = FileSystem::Load(assets_file, &fileBuffer);

	switch (type)
	{
	case RESOURCE_MODEL: 
		ModelImporter::Import(fileBuffer, (ResourceModel*)resource, size); break;
	case RESOURCE_TEXTURE:
		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size); break;
	case RESOURCE_SCENE: 
		break;
	default: break;
	}

	if (resource == nullptr)
		return -1;

	SaveResource(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
}

uint ModuleResources::ImportInternalResource(const char* path, const void* data, ResourceType type)
{
	uint ret = 0;
	Resource* resource = CreateResource(path, type);

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

void ModuleResources::CreateResourceData(uint UID, const char* assets_path, const char* library_path)
{
	resources_data[UID].assetsFile = assets_path;
	resources_data[UID].libraryFile = library_path;

}

void ModuleResources::AddResourceToDelete(uint UID)
{
	_toDeleteResource = UID;
	App->AddModuleToTaskStack(this);
}

bool ModuleResources::DeleteAssetsResource(const char* assets_path)
{
	bool ret = true;

	int UID = Find(assets_path);

	if (UID != -1)
	{
		AddResourceToDelete(UID);
	}

//	FileSystem::Delete(assets_path);

	return ret;
}

bool ModuleResources::DeleteResource(uint UID)
{
	bool ret = true;
	std::string assets_file = resources_data[UID].assetsFile;
	std::string meta_file = assets_file + ".meta";

	switch (resources_data[UID].type)
	{
	case ResourceType::RESOURCE_MODEL:
		DeleteInternalResources(UID);
		break;
	case ResourceType::RESOURCE_TEXTURE:
		break;
	default:
		break;
	}

	FileSystem::Delete(resources_data[UID].assetsFile.c_str());
	FileSystem::Delete(resources_data[UID].libraryFile.c_str());
	FileSystem::Delete(meta_file.c_str());

	ReleaseResource(UID);
	//ReleaseResourceData(resource->GetUID());

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

	std::string library_path = Find(UID);
	ResourceType type = GetTypeFromPath(library_path.c_str());

	if (type == ResourceType::RESOURCE_MESH) 
	{
		ret = FileSystem::Delete(library_path.c_str());
	}
	else if (type == ResourceType::RESOURCE_MATERIAL) 
	{
		const char* texture_library_path = MaterialImporter::ExtractTexture(library_path.c_str());
		DeleteResource(Find(texture_library_path));
		ret = FileSystem::Delete(library_path.c_str());
	}

	ReleaseResource(UID);
	return ret;
}

Resource* ModuleResources::LoadResource(uint UID, ResourceType type)
{
	Resource* resource = CreateResource(UID, type);

	switch (resource->GetType())
	{
	case RESOURCE_MODEL:
		ModelImporter::Load(resource->libraryFile.c_str(), (ResourceModel*)resource);
		break;
	case RESOURCE_MESH:
		MeshImporter::Load(resource->libraryFile.c_str(), (ResourceMesh*)resource);
		break;
	case RESOURCE_MATERIAL:
		MaterialImporter::Load(resource->libraryFile.c_str(), (ResourceMaterial*)resource);
		break;
	case RESOURCE_TEXTURE:
		TextureImporter::Load(resource->libraryFile.c_str(), (ResourceTexture*)resource);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	return resources[UID];
}

void ModuleResources::UnloadResource(uint UID)
{
	delete resources[UID];
	resources[UID] = nullptr;

	resources.erase(resources.find(UID));
}

Resource* ModuleResources::CreateResource(const char* assetsPath, ResourceType type)
{
	Resource* resource = nullptr;
	uint UID = GenerateUID();

	char* buffer;
	uint size = FileSystem::Load(assetsPath, &buffer);

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
		resources[UID] = resource;
		resource->assetsFile = assetsPath;
		resource->libraryFile = GenerateLibraryPath(resource);

		resources_data[UID].assetsFile = resource->assetsFile;
		resources_data[UID].libraryFile = resource->libraryFile;
		resources_data[UID].type = type;
	}

	RELEASE_ARRAY(buffer);
	return resource;
}

Resource* ModuleResources::CreateResource(uint UID, ResourceType type)
{
	Resource* resource = nullptr;

	//ResourceType type = resources_data[UID].type;

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
		resources[UID] = resource;
		resource->assetsFile = resources_data[UID].assetsFile;
		resource->libraryFile = resources_data[UID].libraryFile;
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
	ResourceType type = GetTypeFromPath(library_file);

	Resource* resource = LoadResource(UID, type);
	
	if (resource != nullptr)
		resource->referenceCount++;

	return resource;
}

GameObject* ModuleResources::RequestGameObject(const char* assets_file)
{
	std::string meta_file = assets_file;
	meta_file.append(".meta");
	ResourceModel* model = (ResourceModel*)RequestResource(GetUIDFromMeta(meta_file.c_str()));

	return ModelImporter::ConvertToGameObject(model);
}

void ModuleResources::ReleaseResource(uint UID)
{
	std::map<uint, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
	{
		it->second->referenceCount--;

		if (it->second->referenceCount <= 0)
			UnloadResource(UID);
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

ResourceType ModuleResources::GetTypeFromPath(const char* path)
{
	std::string extension = FileSystem::GetFileFormat(path);
	
	if (extension == ".fbx" || extension == ".model") { return ResourceType::RESOURCE_MODEL; }
	else if (extension == ".mesh") { return ResourceType::RESOURCE_MESH; }
	else if (extension == ".material") { return ResourceType::RESOURCE_MATERIAL; }
	else if (extension == ".png" || extension == ".dds") { return ResourceType::RESOURCE_TEXTURE; }
	else return ResourceType::RESOURCE_UNKNOWN;
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

const char* ModuleResources::GenerateAssetsPath(const char* path)
{
	ResourceType type = GetTypeFromPath(path);
	std::string file = FileSystem::GetFile(path) + FileSystem::GetFileFormat(path);

	char* library_path = new char[128];

	switch (type)
	{
	case RESOURCE_MODEL:
		sprintf_s(library_path, 128, "Assets/Models/%s", file.c_str()); break;
	case RESOURCE_TEXTURE:
		sprintf_s(library_path, 128, "Assets/Textures/%s", file.c_str()); break;
	case RESOURCE_SCENE:
		sprintf_s(library_path, 128, "Assets/Scenes/%s", file.c_str()); break;
	default:
		break;
	}

	return library_path;
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

		if (file.find(".meta") != std::string::npos)
			continue;

		std::string meta = file;
		meta.append(".meta");

		if(FileSystem::Exists(meta.c_str()))
		{
			if (!MetaUpToDate(file.c_str(), meta.c_str()))
			{
				UpdateAssetsResource(file.c_str());
			}
		}
		else 
		{
			ImportFile(file.c_str());
		}
	}
}

/*
std::string str = files[i];
std::size_t found = str.find(".meta");


if (found == -1) {
	std::string meta = directory;
	meta.append("/" + files[i] + ".meta");
	if (!FileSystem::Exists(meta.c_str())) {
		std::string file_to_import = directory;
		file_to_import.append("/");
		file_to_import.append(files[i].c_str());
		ImportFile(file_to_import.c_str());
	}
	else
	{
		std::string file = directory;
		file.append("/" + files[i]);
		MetaUpToDate(file.c_str());
	}
}
*/