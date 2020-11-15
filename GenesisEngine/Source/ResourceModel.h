#pragma once
#include "Globals.h"
#include "Resource.h"
#include "GnJSON.h"

#include <vector>

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(GnJSONObj& base_object) override;
	uint SaveMeta(GnJSONObj& base_object, uint last_modification) override;

public:
	GnJSONObj model_information;

	std::vector<uint> meshes;
	std::vector<uint> textures;
};