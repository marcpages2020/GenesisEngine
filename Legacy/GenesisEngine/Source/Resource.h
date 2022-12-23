#pragma once
#include "Globals.h"

#include <string>

class GnJSONObj;

enum ResourceType 
{
	RESOURCE_MODEL,
	RESOURCE_MESH,
	RESOURCE_MATERIAL,
	RESOURCE_TEXTURE,
	RESOURCE_SCENE,
	RESOURCE_SHADER,
	RESOURCE_UNKNOWN
};

class Resource 
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	ResourceType GetType();
	uint GetUID();
	void SetUID(uint UID);

	virtual uint Save(GnJSONObj& base_object) { return -1; };
	virtual uint SaveMeta(GnJSONObj& base_object, uint last_modification) { return -1; };
	virtual void Load(GnJSONObj& base_object) {};

public: 
	std::string name;
	std::string assetsFile;
	std::string libraryFile;

	int referenceCount = 0;

protected:
	uint _uid = 0;
	ResourceType _type = RESOURCE_UNKNOWN;
};
