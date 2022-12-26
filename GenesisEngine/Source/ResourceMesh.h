#pragma once
#include "Resource.h"

#define VERTEX_ATTRIBUTES 14

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint newUid);
	~ResourceMesh();

public:
	char* name;
	uint numVertices;
	float* vertices;
	uint numIndices;
	uint* indices;
};

