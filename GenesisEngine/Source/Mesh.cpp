#include "Mesh.h"
#include "glew/include/glew.h"

// Mesh ===================================== 

Mesh::Mesh() {}

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

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

SimplePlane::SimplePlane() : Mesh() {}

SimplePlane::~SimplePlane() {}

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

Pyramid::Pyramid() : Mesh() {}

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

// ------------------------------------------

// Sphere ===================================

Sphere::Sphere() : Mesh() 
{
	float radius = 1;
	unsigned int rings = 12;
	unsigned int sectors = 24;

	float const R = 1.0f / (float)(rings - 1);
	float const S = 1.0f / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = sin( -M_PI * 0.5f + M_PI * r * R);
			float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2*M_PI * s * S) * sin(M_PI * r * R);

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}
	}

	indices.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = indices.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
	}
}

Sphere::~Sphere()
{
	vertices.clear();
	indices.clear();
}

void Sphere::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glDrawElements(GL_QUADS, indices.size() , GL_UNSIGNED_SHORT, &indices[0]);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// ------------------------------------------

// Cylinder =================================

Cylinder::Cylinder() : Mesh(), radius(1), height(1), sides(16)  {
	CalculateGeometry();
}

Cylinder::Cylinder(float g_radius, float g_height, int g_sides) : Mesh(), radius(g_radius), height(g_height), sides(g_sides) {
	CalculateGeometry();
}

Cylinder::~Cylinder() 
{
	delete[] vertices;
	delete[] indices;
}

void Cylinder::CalculateGeometry()
{
	float current_angle = 0;
	float angle_increase = 2 * M_PI / sides;

	//Vertices ------------------------------------------------

	std::vector<float> vertices_vector;

	//Top center
	vertices_vector.push_back(0);
	vertices_vector.push_back(height * 0.5f);
	vertices_vector.push_back(0);

	//Top face
	for (int i = 0; i < sides; i++)
	{
		vertices_vector.push_back(radius * cos(current_angle));//x
		vertices_vector.push_back(height * 0.5f);		       //y
		vertices_vector.push_back(radius * sin(current_angle));//z

		//anticlockwise
		current_angle -= angle_increase;
	}

	current_angle = 0;

	//Bottom Center
	vertices_vector.push_back(0);
	vertices_vector.push_back(-height * 0.5f);
	vertices_vector.push_back(0);

	//Bottom face
	for (int i = 0; i < sides; i++)
	{
		vertices_vector.push_back(radius * cos(current_angle)); //x
		vertices_vector.push_back(-height * 0.5f);			    //y
		vertices_vector.push_back(radius * sin(current_angle)); //z

		//clockwise
		current_angle -= angle_increase;
	}

	// Indices ----------------------------------------------

	std::vector<uint> indices_vector;

	//Top Face
	for (int i = 1; i < sides; i++)
	{
		indices_vector.push_back(0);
		indices_vector.push_back(i);
		indices_vector.push_back(i + 1);
	}

	indices_vector.push_back(0);
	indices_vector.push_back(sides);
	indices_vector.push_back(1);

	//Sides
	for (int i = 1; i < sides; i++)
	{
		//Left triangle
		indices_vector.push_back(i);
		indices_vector.push_back(sides + i + 1);
		indices_vector.push_back(sides + i + 2);

		//Right triangle
		indices_vector.push_back(i + sides + 2);
		indices_vector.push_back(i + 1);
		indices_vector.push_back(i);
	}

	indices_vector.push_back(sides);
	indices_vector.push_back(2 * sides + 1);
	indices_vector.push_back(sides + 2);

	indices_vector.push_back(sides + 2);
	indices_vector.push_back(1);
	indices_vector.push_back(sides);

	//Bottom Face
	int k = sides + 1;
	for (int i = 1; i < sides; i++)
	{
		indices_vector.push_back(k);
		indices_vector.push_back(k + i + 1);
		indices_vector.push_back(k + i);
	}

	indices_vector.push_back(k);
	indices_vector.push_back(sides + 2);
	indices_vector.push_back(2 * sides + 1);

	vertices_amount = vertices_vector.size();
	vertices = new float[vertices_amount]();

	for (size_t i = 0; i < vertices_amount; i++)
	{
		vertices[i] = vertices_vector[i];
	}

	indices_amount = indices_vector.size();
	indices = new uint[indices_amount]();

	for (size_t i = 0; i < indices_amount; i++)
	{
		indices[i] = indices_vector[i];
	}

	vertices_vector.clear();
	indices_vector.clear();
}

void Cylinder::Render()
{
	FullRender(vertices, vertices_amount, indices, indices_amount);
}

// ------------------------------------------

// Grid =====================================

Grid::Grid(int g_size) {
	if ((g_size % 2) != 0)
		g_size++;

	size = g_size;
}

Grid::~Grid() {}

void Grid::Render()
{
	glBegin(GL_LINES);

	//Vertical Lines
	for (float x = -size * 0.5f; x <= size * 0.5f; x++)
	{	
		glVertex3f(x, 0, -size * 0.5f);
		glVertex3f(x, 0, size * 0.5f);
	}

	//Hortiontal Lines
	for (float z = -size * 0.5f; z <= size * 0.5f; z++)
	{
		glVertex3f(-size * 0.5f, 0, z);
		glVertex3f(size * 0.5f, 0, z);
	}

	glEnd();
}