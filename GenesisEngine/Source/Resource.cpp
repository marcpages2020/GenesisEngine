#include "Resource.h"

Resource::Resource(uint newUid) : uid(newUid), type(ResourceType::UNKNOWN)
{}

Resource::Resource(uint newUid, ResourceType newType) : uid(newUid), type(newType)
{}

Resource::~Resource()
{
	uid = -1;
	type = ResourceType::UNKNOWN;
}

ResourceType Resource::GetType() const { return type; }

uint Resource::GetUID() const {	return uid; }

void Resource::SetUID(uint newUid) { uid = newUid; }
