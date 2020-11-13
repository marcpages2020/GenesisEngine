#include "ModuleResources.h"
#include "GameObject.h"
#include "FileSystem.h"
#include "Importers.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Resource.h"

ModuleResources::ModuleResources(bool start_enabled) : Module(start_enabled)
{
	name = "resources";
}

ModuleResources::~ModuleResources() {}

void ModuleResources::ImportFile(const char* file_path, bool drag_and_drop)
{
	std::string processed_path = FileSystem::ProcessPath(file_path);
	ResourceType type = GetResourceTypeFromPath(file_path);

	switch (type)
	{
	case TEXTURE:
		break;
	case MESH:
		break;
	case SCENE:
		break;
	case MODEL:
		ModelImporter::Import(processed_path.c_str());
		break;
	case UNKNOWN:
		break;
	default:
		break;
	}

	/*
	if (extension == ".fbx")
	{
		GameObject* imported_fbx = MeshImporter::ImportFBX(processed_path.c_str());

		if (imported_fbx != nullptr)
			App->scene->AddGameObject(imported_fbx);
	}
	else if (extension == ".png" || extension == ".jpg" || extension == ".bmp" || extension == ".tga")
	{
		if (drag_and_drop)
		{
			//App->scene->SetDroppedTexture(TextureImporter::LoadTexture(processed_path.c_str()));
		}
	}
	else if (extension == ".scene")
	{
		App->Load(file_path);
	}
	else if (extension == ".model")
	{
		//LOAD MODEL
	}
	else if (extension == ".mesh")
	{
		//LOAD MESH
	}
	else if (extension == ".dds")
	{
		//LOAD TEXTURE
	}
	*/
}

ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{

	std::string extension = FileSystem::GetFileFormat(path);
	
	if (extension == ".fbx") { return ResourceType::MODEL; }
	else if (extension == ".mesh") { return ResourceType::MESH; }
	else if (extension == ".png") { return ResourceType::TEXTURE; }
}
