#pragma once
#include "Globals.h"

#include <string>

enum ResourceType 
{
	TEXTURE,
	MESH,
	SCENE,
	MODEL,
	UNKNOWN
};

class Resource 
{
public:
	Resource(uint uid, ResourceType type);
	~Resource();

protected:
	uint _uid = 0;
	std::string _assetsFile;
	std::string _libraryFile;

	ResourceType _type = UNKNOWN;
	uint _referenceCount = 0;
};
