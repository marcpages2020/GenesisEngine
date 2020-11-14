#pragma once
#include "Globals.h"
#include "Resource.h"

class ResourceModel : public Resource {
public:
	ResourceModel(uint UID);
	~ResourceModel();

	uint Save(GnJSONObj& base_object) override;
};