#include "ResourceMaterial.h"
#include "Engine.h"

ResourceMaterial::ResourceMaterial(uint UID) : Resource(UID, ResourceType::RESOURCE_MATERIAL), diffuseMapID(0), normalMapID(0), diffuseMap(nullptr), normalMap(nullptr) 
{
	//diffuseColor = Color(0.0, 1.0, 0.0, 1.0);
	tiling[TextureType::DIFFUSE_MAP][0] = tiling[TextureType::DIFFUSE_MAP][1] = 1.0f;
	tiling[TextureType::NORMAL_MAP][0] = tiling[TextureType::NORMAL_MAP][1] = 1.0f;
}

ResourceMaterial::~ResourceMaterial()
{}

