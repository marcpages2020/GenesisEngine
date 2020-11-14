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
	MaterialImporter::Init();

	return ret;
}

uint ModuleResources::ImportFile(const char* assets_file, bool drag_and_drop)
{
	std::string processed_path = FileSystem::ProcessPath(assets_file);
	ResourceType type = GetResourceTypeFromPath(assets_file);

	Resource* resource = CreateResource(assets_file, type);
	uint ret = 0;
	
	char* fileBuffer;
	uint size = FileSystem::Load(assets_file, &fileBuffer);

	switch (type)
	{
	case MODEL:
		ModelImporter::Import(processed_path.c_str());
		break;
	case MESH:
		//MeshImporter::Import(processed_path.c_str());
		break;
	case TEXTURE:
		//TextureImporter
		break;
	case SCENE:
		break;
	case UNKNOWN:
		break;
	default:
		break;
	}

	SaveResource(resource);
	ret = resource->GetUID();
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
	case MODEL:
		resource = new ResourceModel(UID);
		break;
	case MESH:
		resource = new ResourceMesh(UID);
		break;
	case TEXTURE:
		resource = new ResourceTexture(UID);
		break;
	case SCENE:
		break;
	case UNKNOWN:
		break;
	default:
		break;
	}

	if (resource != nullptr)
	{
		resources[UID] = resource;
		resource->_assetsFile = assetsPath;
		resource->_libraryFile = GenerateLibraryPath(resource);
	}

	RELEASE_ARRAY(buffer);
	return resource;
}

bool ModuleResources::SaveResource(Resource* resource)
{
	bool ret = true;

	GnJSONObj base_object;
	resource->Save(base_object);
	
	char* buffer = NULL;
	uint size = base_object.Save(&buffer);
	FileSystem::Save(resource->_libraryFile.c_str(), buffer, size);

	base_object.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{

	std::string extension = FileSystem::GetFileFormat(path);
	
	if (extension == ".fbx") { return ResourceType::MODEL; }
	else if (extension == ".mesh") { return ResourceType::MESH; }
	else if (extension == ".png") { return ResourceType::TEXTURE; }
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
	case MODEL:
		sprintf_s(library_path, 128, "Library/Models/%d.model", resource->GetUID());
		break;
	case MESH:
		sprintf_s(library_path, 128, "Library/Meshes/%d.mesh", resource->GetUID());
		break;
	case TEXTURE:
		sprintf_s(library_path, 128, "Library/Textures/%d.dds", resource->GetUID());
		break;
	case SCENE:
		sprintf_s(library_path, 128, "Library/Scenes/%d.scene", resource->GetUID());
		break;
	case UNKNOWN:
		break;
	default:
		break;
	}

	return library_path;
}

