#include "ModuleResources.h"

#include "Importer.h"
#include "ResourceModel.h"

ModuleResources::ModuleResources(GnEngine* app, bool start_enabled) : Module(app, start_enabled)
{}

bool ModuleResources::Start()
{
	ResourceModel* resourceModel = new ResourceModel(0);
	Importer::ImportModel("Assets/Models/Rayman/rayman.fbx", resourceModel);
	delete resourceModel;
	return true;
}
