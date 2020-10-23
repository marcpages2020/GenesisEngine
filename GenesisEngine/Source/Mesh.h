#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

struct GnTexture {
	uint id = -1;
	GLubyte* data = nullptr;
	int width = -1;
	int height = -1;
	//const char* name;
};

class GnMesh {
public:
	GnMesh();
	virtual ~GnMesh();

	void GenerateBuffers();
	void AssignTexture(GnTexture texture);
	void AssingCheckersImage();
	virtual void Render();
	void DrawVertexNormals();
	void DrawFaceNormals();

public:
	uint vertices_buffer = -1;
	int vertices_amount = -1;
	float* vertices = nullptr;

	uint indices_buffer = -1;
	int indices_amount = -1;
	uint* indices = nullptr;

	uint normals_buffer;
	float* normals;

	uint texture_buffer = -1;
	uint textureID;
	GnTexture texture;

	float* texcoords = nullptr;

	float* colors;
};

class GnCube : public GnMesh {
public:
	GnCube();
	~GnCube();
};

class GnPlane : public GnMesh {
public:
	GnPlane();
	~GnPlane();
};

class GnPyramid : public GnMesh {
public:
	GnPyramid();
	~GnPyramid();
};

class GnSphere : public GnMesh {
public:
	GnSphere();
	~GnSphere();

	void Render() override;

private: 
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
};

class GnCylinder : public GnMesh {
public:
	GnCylinder();
	GnCylinder(float radius, float height, int sides);
	~GnCylinder();
	
	void CalculateGeometry();
private:
	float radius = 0;
	float height = 0;
	unsigned int sides = 0;
};

class GnGrid {
public:
	GnGrid(int size);
	~GnGrid();

	void Render();

private:
	int size;
};

class GnCone : public GnMesh {
public:
	GnCone();
	GnCone(float radius, float height, int sides);
	~GnCone();

	void CalculateGeometry(int sides);

private: 
	float radius;
	float height;
};

class GnMeshCollection {
public:
	GnMeshCollection();
	~GnMeshCollection();

	void GenerateBuffers();
	void Render();

public:
	std::vector<GnMesh*> meshes;
};

/*
class Frustum : public GnMesh {
public:
private:
};
*/