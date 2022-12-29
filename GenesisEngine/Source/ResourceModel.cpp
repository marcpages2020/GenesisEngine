#include "ResourceModel.h"

ResourceModel::ResourceModel(uint newUid) : Resource(newUid, ResourceType::MODEL)
{}

ResourceModel::~ResourceModel()
{
	for (size_t i = 0; i < meshesResources.size(); i++)
	{
		delete meshesResources[i];
	}

	meshesResources.clear();
	meshes.clear();
}

void ResourceModel::AddMesh(uint newMesh)
{
	meshes.push_back(newMesh);
}

int ResourceModel::GetMeshAt(uint meshIndex)
{
	if (meshIndex > meshes.size()) { return -1; }

	return meshes[meshIndex];
}
