#pragma once

typedef unsigned int uint;

enum class ResourceType
{
	MODEL,
	MESH,
	MATERIAL,
	TEXTURE,
	SCENE,
	SHADER,
	UNKNOWN
};

class Resource
{
public:
	Resource(uint newUid);
	Resource(uint newUid, ResourceType newType);
	~Resource();

	ResourceType GetType() const;
	uint GetUID() const;
	void SetUID(uint newUid);

private:
	uint uid;
	ResourceType type;
};

