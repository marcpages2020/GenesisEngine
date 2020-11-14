#include "Resource.h"

Resource::Resource(uint uid, ResourceType type)
{
	_uid = uid;
	_type = type;
}

Resource::~Resource()
{
	_assetsFile.clear();
	_libraryFile.clear();
}

ResourceType Resource::GetType()
{
	return _type;
}

uint Resource::GetUID()
{
	return _uid;
}
