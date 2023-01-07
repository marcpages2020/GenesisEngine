#pragma once
#include "Resource.h"

#define VERTEX_ATTRIBUTES 14

//Offsets
#define VERTEX_POSITION_OFFSET 0 
#define VERTEX_NORMALS_OFFSET 3 
#define VERTEX_TEXCOORDS_OFFSET 6 
#define VERTEX_COLOR_OFFSET 8 
#define VERTEX_TANGENTS_OFFSET 11 

class GnJSONObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

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