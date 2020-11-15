#pragma once
#include "Globals.h"
#include "Resource.h"

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(uint UID);
	~ResourceMaterial();

public:
	uint diffuse_texture_uid;
};
