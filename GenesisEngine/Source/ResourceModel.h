#pragma once
#include "Globals.h"
#include "Resource.h"
#include "GnJSON.h"

#include <vector>

struct ModelNode
{
	std::string name;
	float3 position;
	Quat rotation;
	float3 scale;

	int meshID = -1;
	int materialID = -1;

	uint UID;
	uint parentUID;
};

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(GnJSONObj& base_object) override;
	uint SaveMeta(GnJSONObj& base_object, uint last_modification) override;

public:
	std::vector<ModelNode> nodes;
	std::vector<uint> meshes;
	std::vector<uint> materials;
	std::vector<uint> textures;
	//std::vector<Light> lights;
	//std::vector<Camera> cameras;
};