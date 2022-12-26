#include "ResourceModel.h"

ResourceModel::ResourceModel(uint newUid) : Resource(newUid, ResourceType::MODEL)
{}

ResourceModel::~ResourceModel()
{}

void ResourceModel::AddMesh(uint newMesh)
{
	meshes.push_back(newMesh);
}
