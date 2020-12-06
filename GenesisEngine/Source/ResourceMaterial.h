#pragma once
#include "Globals.h"
#include "Resource.h"
#include "Color.h"

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(uint UID);
	~ResourceMaterial();

public:
	uint diffuseTextureUID;
	Color diffuseColor;
};
