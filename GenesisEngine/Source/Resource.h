#pragma once
#include "Globals.h"

#include <string>

class GnJSONObj;

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
	virtual ~Resource();

	ResourceType GetType();
	uint GetUID();

	virtual uint Save(GnJSONObj& base_object) { return -1; };
	virtual void Load(GnJSONObj& base_object) {};

public: 
	std::string _assetsFile;
	std::string _libraryFile;

protected:
	uint _uid = 0;

	ResourceType _type = UNKNOWN;
	uint _referenceCount = 0;
};
