#include "Resource.h"

Resource::Resource(uint uid, ResourceType type) : referenceCount(0), _uid(uid), _type(type)
{}

Resource::~Resource()
{
	name.clear();
	assetsFile.clear();
	libraryFile.clear();

	_uid = 0;
}

ResourceType Resource::GetType() {	return _type; }

uint Resource::GetUID() { return _uid; }

void Resource::SetUID(uint UID) { _uid = UID; }
