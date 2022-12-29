#include "ModuleScene.h"
#include "ResourceModel.h"
#include "FileSystem.h"
#include "Importer.h"

#include "Engine.h"

#include "ResourceModel.h"

ModuleScene::ModuleScene(GnEngine* app, bool start_enabled) : Module(app, start_enabled), resourceModel(nullptr)
{}

bool ModuleScene::Start()
{
	resourceModel = new ResourceModel(0);
	char* buffer;
	//int fileSize = FileSystem::Load("Assets/Models/baker_house/BakerHouse.fbx", &buffer);
	int fileSize = FileSystem::Load("Assets/Models/Rayman/rayman.fbx", &buffer);
	Importer::ImportModel(buffer, fileSize, resourceModel);

	return true;
}

update_status ModuleScene::Update(float deltaTime)
{
	
	for (size_t i = 0; i < resourceModel->meshesResources.size(); i++)
	{
		engine->renderer3D->AddMeshToRender(resourceModel->meshesResources[i]);
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	delete resourceModel;
	return true;
}
