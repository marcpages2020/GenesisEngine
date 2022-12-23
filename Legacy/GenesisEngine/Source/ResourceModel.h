#pragma once
#include "Globals.h"
#include "Resource.h"
#include "GnJSON.h"
#include <vector>

class Light;
class Camera;

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
	std::vector<Light*> lights;
	std::vector<Camera*> cameras;
};