#pragma once
#include "Globals.h"
#include "Resource.h"
#include "Color.h"
#include "ResourceTexture.h"

class ResourceTexture;

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(uint UID);
	~ResourceMaterial();

public:
	uint diffuseMapID;
	ResourceTexture* diffuseMap;
	uint normalMapID;
	ResourceTexture* normalMap;
	float tiling[TextureType::UNKNOWN_MAP][2];

	Color diffuseColor;
};
