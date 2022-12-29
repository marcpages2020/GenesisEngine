#pragma once
#include "Resource.h"

#define VERTEX_ATTRIBUTES 14

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint newUid);
	~ResourceMesh();

	void GenerateBuffers();
	void DeleteBuffers();

public:
	char* name;
	uint numVertices;
	float* vertices;
	uint numIndices;
	uint* indices;

	uint VAO;
	uint VBO;
	uint EBO;
};

