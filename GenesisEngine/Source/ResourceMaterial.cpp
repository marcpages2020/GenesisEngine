#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(uint UID) : Resource(UID, ResourceType::RESOURCE_MATERIAL), diffuseMapID(0), diffuseMap(nullptr), normalMap(nullptr) {}

ResourceMaterial::~ResourceMaterial() {}

