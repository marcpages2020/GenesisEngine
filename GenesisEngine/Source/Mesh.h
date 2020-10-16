#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	void GenerateBuffers();
	virtual void Render();

public:
	uint vertices_buffer = 0;
	int vertices_amount = 0;
	float* vertices = nullptr;

	uint indices_buffer = 0;
	int indices_amount = 0;
	uint* indices = nullptr;
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

class ImportedMesh : public Mesh {
public:
	ImportedMesh();
	~ImportedMesh();

	void Render() override;
};

/*
class Frustum : public Mesh {
public:
private:
};
*/