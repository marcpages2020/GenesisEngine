#pragma once
#include "Globals.h"
#include <vector>;

typedef float GLfloat;
typedef unsigned short GLushort;

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	virtual void Render() = 0;

public: 

protected: 
	void FullRender(float vertices[], int vertices_amount, uint indices[], int indices_amount);
};

class Cube : public Mesh {
public:
	Cube();
	Cube(float x, float y, float z);
	~Cube();

	void Render();
};

class SimplePlane : public Mesh {
public:
	SimplePlane();
	~SimplePlane();

	void Render();
};

class Pyramid : public Mesh {
public:
	Pyramid();
	~Pyramid();

	void Render();
};

class Sphere : public Mesh {
public:
	Sphere();
	~Sphere();

	void Render();

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
	void Render();
private:
	float radius;
	float height;
	unsigned int sides;

	float* vertices;
	int vertices_amount;

	uint* indices;
	int indices_amount;
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
	void Render();

private: 
	float radius;
	float height;

	float* vertices;
	int vertices_amount;

	uint* indices;
	uint indices_amount;
};

/*
class Frustum : public Mesh {
public:
private:
};
*/