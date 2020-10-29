#pragma once
#include "Globals.h"
#include "Component.h"
#include <vector>;
#include "Material.h"

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

class GnMesh : public Component {
public:
	GnMesh();
	virtual ~GnMesh();

	void GenerateBuffers();

	bool SetTexture(GnTexture* texture);
	void AssignCheckersImage();

	virtual void Update() override;
	virtual void Render();
	virtual void OnEditor() override;

	void RemoveTexture();

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
	GnTexture* texture;
	float* texcoords = nullptr;
	float* colors;

	const char* name;

private:
	bool draw_vertex_normals;
	bool draw_face_normals;
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

class GnMeshCollection : public Component{
public:
	GnMeshCollection();
	~GnMeshCollection();

	void GenerateBuffers();
	virtual void Update() override;
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