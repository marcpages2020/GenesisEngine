#include "Mesh.h"
#include "glew/include/glew.h"


// Mesh ===================================== 

Mesh::Mesh() {
}

Mesh::~Mesh(){}

void Mesh::FullRender(float vertices[], int vertices_amount, uint indices[], int indices_amount)
{
	uint vertices_buffer = 0;
	glGenBuffers(1, (GLuint*)&(vertices_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount * 3, vertices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	uint indices_buffer = 0;
	glGenBuffers(1, (GLuint*)&(indices_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_amount, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glDrawElements(GL_TRIANGLES, indices_amount, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

// ------------------------------------------

// Cube ===================================== 

Cube::Cube() : Mesh() {}

Cube::Cube(float g_x, float g_y, float g_z) {}

Cube::~Cube(){}

void Cube::Render()
{
	float vertices[24] = {
		//Bottom Vertices
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f,

		//Top Vertices
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f  
	};

	uint indices[36] = {
		//Bottom face
		0,1,2, 2,3,0,
		//Front Face
		3,2,6, 6,7,3,
		//Right face
		6,2,1, 1,5,6,
		//Left face
		4,0,7, 7,0,3,
		//Back face
		1,0,5, 0,4,5,
		//Top face
		4,7,6, 6,5,4
	};

	FullRender(vertices, 24, indices, 36);
}

// ------------------------------------------

// Plane ====================================

SimplePlane::SimplePlane()
{
}

SimplePlane::~SimplePlane()
{
}

void SimplePlane::Render()
{
	float vertices[12] = {
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f,
	};

	uint indices[6]{
		0, 3, 2,
		2, 1 ,0
	};

	FullRender(vertices, 12, indices, 6);
}

// ------------------------------------------

// Pyramid ==================================

Pyramid::Pyramid() {}

Pyramid::~Pyramid() {}

void Pyramid::Render()
{
	float vertices[15] = {
		//Bottom 
		0.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 0.0f,
		1.0f ,0.0f, 1.0f,
		0.0f ,0.0f, 1.0f,

		//Top
		0.5f, 0.85f, 0.5f
	};

	uint indices[20]{
		//Bottom
		0, 1, 2,
		2, 3, 0,

		3, 2, 4, // Front
		0, 3, 4, // Left
		2, 1, 3, // Right
		1, 0, 4  // Back
	};

	FullRender(vertices, 15, indices, 18);
}

