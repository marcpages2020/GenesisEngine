#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(uint UID) : Resource(UID, ResourceType::RESOURCE_MATERIAL), diffuseTextureUID(0) {}

ResourceMaterial::~ResourceMaterial() {}

