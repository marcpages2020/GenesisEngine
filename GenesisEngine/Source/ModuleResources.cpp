#include "ModuleResources.h"

#include "Importer.h"
#include "ResourceModel.h"

#include "FileSystem.h"

ModuleResources::ModuleResources(GnEngine* app, bool start_enabled) : Module(app, start_enabled)
{}

bool ModuleResources::Start()
{
	return true;
}
