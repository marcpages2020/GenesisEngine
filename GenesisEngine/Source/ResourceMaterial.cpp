#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(uint UID) : Resource(UID, ResourceType::RESOURCE_MATERIAL), diffuse_texture_uid(0) {}

ResourceMaterial::~ResourceMaterial() {}
