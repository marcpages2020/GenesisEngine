#pragma once
#include "Resource.h"
#include <vector>

class ResourceModel : public Resource
{
public:
	ResourceModel(uint newUid);
	~ResourceModel();

	void AddMesh(uint newMesh);

private: 
	std::vector<uint> meshes;
};

