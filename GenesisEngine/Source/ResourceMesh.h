#pragma once
#include "Resource.h"

#define VERTEX_ATTRIBUTES 14

#define VERTEX_POSITION_OFFSET 0 
#define VERTEX_NORMALS_OFFSET 3 
#define VERTEX_TEXCOORDS_OFFSET 6 
#define VERTEX_COLOR_OFFSET 8 
#define VERTEX_TANGENTS_OFFSET 11 


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

