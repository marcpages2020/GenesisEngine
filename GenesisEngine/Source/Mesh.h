#pragma once
#include "Globals.h"

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

