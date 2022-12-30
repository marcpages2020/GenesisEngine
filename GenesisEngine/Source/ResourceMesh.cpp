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
	//generate vertices array
	glGenVertexArrays(1, &VAO);
	
	//generate buffers
	glGenBuffers(1, &VBO); //vertices buffer
	glGenBuffers(1, &EBO); //indices buffer
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //send vertices array to the VRAM

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); //send indices to the VRAM

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)VERTEX_POSITION_OFFSET);
	glEnableVertexAttribArray(0);

	//normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(VERTEX_NORMALS_OFFSET * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//texcoords attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(VERTEX_TEXCOORDS_OFFSET * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//color attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(VERTEX_COLOR_OFFSET * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	//tangents attribute
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(VERTEX_TANGENTS_OFFSET * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
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
