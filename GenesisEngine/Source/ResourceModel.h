#pragma once
#include "Resource.h"
#include <vector>

#include "HelperClasses/GnJSON.h"

class ResourceMesh;

struct ModelNode
{
	std::string name;
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	int meshID = -1;
	int materialID = -1;

	uint UID = 0u;
	uint parentUID = 0u;
};

class ResourceModel : public Resource
{
public:
	ResourceModel(uint newUid);
	~ResourceModel();

	void AddMesh(uint newMesh);
	int GetMeshAt(uint meshIndex);

	std::vector<ResourceMesh*> meshesResources;
private: 
	std::vector<uint> meshes;
};

