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

#include "MathGeoLib/include/MathGeoLib.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
	name = "resources";
}

ModuleResources::~ModuleResources() {}

bool ModuleResources::Init()
{
	bool ret = true;

	MeshImporter::Init();
	TextureImporter::Init();

	return ret;
}

bool ModuleResources::MetaUpToDate(const char* asset_path)
{
	std::string meta_file = asset_path;
	meta_file += ".meta";

	if (FileSystem::Exists(meta_file.c_str()))
	{
		char* buffer = nullptr;
		uint size = FileSystem::Load(meta_file.c_str(), &buffer);
		GnJSONObj meta(buffer);

		int lastModifiedMeta = meta.GetInt("lastModified");
		uint lastModified = FileSystem::GetLastModTime(asset_path);

		if (lastModifiedMeta != lastModified)
			return false;
		else
			return true;
	}
	else {
		return false;
	}
}

uint ModuleResources::ImportFile(const char* assets_file)
{
	//if (MetaUpToDate(assets_file))
		//return 0;

	std::string processed_path = FileSystem::ProcessPath(assets_file);

	ResourceType type = GetResourceTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;
	
	char* fileBuffer;
	uint size = FileSystem::Load(assets_file, &fileBuffer);

	switch (type)
	{
	case RESOURCE_MODEL:
		ModelImporter::Import(fileBuffer, (ResourceModel*)resource, size);
		break;
	case RESOURCE_TEXTURE:
		TextureImporter::Import(fileBuffer, (ResourceTexture*)resource, size);
		size = TextureImporter::Save(&fileBuffer, (ResourceTexture*)resource);
		break;
	case RESOURCE_SCENE:
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	SaveResource(fileBuffer, size, resource);
	//SaveMetaFile(resource);
	ret = resource->GetUID();
	ReleaseResource(ret);
	RELEASE_ARRAY(fileBuffer);

	return ret;
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
	}

	RELEASE_ARRAY(buffer);
	return resource;
}

Resource* ModuleResources::RequestResource(uint UID)
{
	Resource* resource = nullptr;
	std::map<uint, Resource*>::iterator it = resources.find(UID);

	if (it != resources.end()) {
		it->second->referenceCount++;
		return it->second;
	}
}

void ModuleResources::ReleaseResource(uint UID)
{
	delete resources[UID];
	resources[UID] = nullptr;
}

bool ModuleResources::SaveResource(char* fileBuffer, uint size, Resource* resource)
{
	bool ret = true;

	ret = SaveMetaFile(resource);

	//if (resource->GetType() != ResourceType::RESOURCE_MODEL && resource->GetType() != ResourceType::RESOURCE_SCENE)
	FileSystem::Save(resource->libraryFile.c_str(), fileBuffer, size);

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

ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{

	std::string extension = FileSystem::GetFileFormat(path);
	
	if (extension == ".fbx") { return ResourceType::RESOURCE_MODEL; }
	else if (extension == ".mesh") { return ResourceType::RESOURCE_MESH; }
	else if (extension == ".png") { return ResourceType::RESOURCE_TEXTURE; }
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
		sprintf_s(library_path, 128, "Library/Models/%d.model", resource->GetUID());
		break;
	case RESOURCE_MESH:
		sprintf_s(library_path, 128, "Library/Meshes/%d.mesh", resource->GetUID());
		break;
	case RESOURCE_MATERIAL:
		sprintf_s(library_path, 128, "Library/Materials/%d.material", resource->GetUID());
		break;
	case RESOURCE_TEXTURE:
		sprintf_s(library_path, 128, "Library/Textures/%d.dds", resource->GetUID());
		break;
	case RESOURCE_SCENE:
		sprintf_s(library_path, 128, "Library/Scenes/%d.scene", resource->GetUID());
		break;
	case RESOURCE_UNKNOWN:
		break;
	default:
		break;
	}

	return library_path;
}

