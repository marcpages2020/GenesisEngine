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
	uint vertices_buffer = 0;
	uint vertices_amount = 0;
	float* vertices = nullptr;

	uint indices_buffer = 0;
	uint indices_amount = 0;
	uint* indices = nullptr;

	uint normals_buffer = 0;
	float* normals;
	uint normals_amount = -1;

	uint texcoords_buffer = 0;
	float* texcoords = nullptr;
	uint texcoords_amount = -1;

	float* colors;
};