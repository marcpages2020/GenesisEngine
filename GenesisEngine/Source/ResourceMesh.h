#pragma once
#include "Resource.h"

class GnJSONObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(GnJSONObj& base_object);

	void GenerateBuffers();
	void DeleteBuffers();

public:
	uint VBO;
	uint VAO;
	uint vertices_amount;
	float* vertices;

	uint EBO;
	uint indices_amount;
	uint* indices;

private:
	bool _buffers_created;
};