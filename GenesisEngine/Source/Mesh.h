#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

struct Texture {
	uint id = -1;
	GLubyte* data;
	int width = -1;
	int height = -1;
};

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	void GenerateBuffers();
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
	Texture texture;

	float* texcoords = nullptr;

	float* colors;
};

class Cube : public Mesh {
public:
	Cube();
	~Cube();
};

class Plane : public Mesh {
public:
	Plane();
	~Plane();
};

class Pyramid : public Mesh {
public:
	Pyramid();
	~Pyramid();
};

class Sphere : public Mesh {
public:
	Sphere();
	~Sphere();

	void Render() override;

private: 
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
};

class Cylinder : public Mesh {
public:
	Cylinder();
	Cylinder(float radius, float height, int sides);
	~Cylinder();
	
	void CalculateGeometry();
private:
	float radius = 0;
	float height = 0;
	unsigned int sides = 0;
};

class Grid {
public:
	Grid(int size);
	~Grid();

	void Render();

private:
	int size;
};

class Cone : public Mesh {
public:
	Cone();
	Cone(float radius, float height, int sides);
	~Cone();

	void CalculateGeometry(int sides);

private: 
	float radius;
	float height;
};

class MeshCollection {
public:
	MeshCollection();
	~MeshCollection();

	void GenerateBuffers();
	void Render();

public:
	std::vector<Mesh*> meshes;
};

/*
class Frustum : public Mesh {
public:
private:
};
*/