#pragma once
#include "Resource.h"

class GnJSONObj;

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint UID);
	~ResourceMesh();

	void Load(GnJSONObj& base_object);

public:
	uint vertices_amount = -1;
	float* vertices = nullptr;

	uint indices_amount = -1;
	uint* indices = nullptr;

	float* normals;
	uint normals_amount = -1;

	float* texcoords = nullptr;
	uint texcoords_amount = -1;

	float* colors;
};