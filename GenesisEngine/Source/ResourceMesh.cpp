#include "ResourceMesh.h"
#include "glew/include/glew.h"

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH),
 VBO(0), VAO(0), EBO(0), 
 vertices_amount(0), vertices(nullptr),
 indices_amount(0), indices(nullptr),
 _buffers_created(false) {}

ResourceMesh::~ResourceMesh()
{
	if(_buffers_created)
		DeleteBuffers();

	delete vertices;
	vertices = nullptr;
	vertices_amount = 0;

	delete indices;
	indices = nullptr;
	indices_amount = 0;
}

void ResourceMesh::GenerateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, (GLuint*)&(VBO));
	glGenBuffers(1, (GLuint*)&(EBO));
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBUTES * vertices_amount, vertices, GL_STATIC_DRAW);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_amount, indices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//texcoords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//normals attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	//tangents attribute
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(11 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);

	_buffers_created = true;
}

void ResourceMesh::DeleteBuffers()
{
	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;
	glDeleteBuffers(1, &EBO);
	EBO = 0u;

	_buffers_created = false;
}

