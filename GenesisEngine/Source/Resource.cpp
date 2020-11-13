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
