#include "ResourceMesh.h"
#include "glad/glad.h"

ResourceMesh::ResourceMesh(uint newUid) : Resource(newUid, ResourceType::MESH), name("None"),
VAO(0u), VBO(0u), EBO(0u), 
numVertices(0u), vertices(nullptr), 
numIndices(0u), indices(nullptr)
{}

ResourceMesh::~ResourceMesh()
{
	delete name;

	delete vertices;
	numVertices = 0u;
	
	delete indices;
	numIndices = 0u;

	DeleteBuffers();
}

void ResourceMesh::GenerateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, (GLuint*)&(VBO));
	glGenBuffers(1, (GLuint*)&(EBO));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBUTES * numVertices, vertices, GL_STATIC_DRAW);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * numIndices, indices, GL_STATIC_DRAW);

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
}

void ResourceMesh::DeleteBuffers()
{
	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;
	glDeleteBuffers(1, &EBO);
	EBO = 0u;
}
