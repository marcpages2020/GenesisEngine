#include "ResourceMesh.h"
#include "glew/include/glew.h"

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH),
 vertices_amount(-1), vertices(nullptr), 
 indices_amount(-1), indices(nullptr),
 normals_amount(-1), normals(nullptr), 
 texcoords(nullptr), colors(nullptr),
 _buffers_created(false) {}

ResourceMesh::~ResourceMesh()
{
	if(_buffers_created)
		DeleteBuffers();

	delete vertices;
	vertices = nullptr;

	delete indices;
	indices = nullptr;

	delete normals;
	normals = nullptr;

	delete texcoords;
	texcoords = nullptr;
}

void ResourceMesh::Load(GnJSONObj& base_object) {}

void ResourceMesh::GenerateBuffers()
{
	//vertices
	glGenBuffers(1, (GLuint*)&(vertices_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount * 3, vertices, GL_STATIC_DRAW);

	//normals
	glGenBuffers(1, (GLuint*)&(normals_buffer));
	glBindBuffer(GL_NORMAL_ARRAY, normals_buffer);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertices_amount * 3, normals, GL_STATIC_DRAW);

	//textures
	glGenBuffers(1, (GLuint*)&(texcoords_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount * 2, texcoords, GL_STATIC_DRAW);

	//indices
	glGenBuffers(1, (GLuint*)&(indices_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_amount, indices, GL_STATIC_DRAW);

	_buffers_created = true;
}

void ResourceMesh::DeleteBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertices_buffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indices_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normals_buffer);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteBuffers(1, &texcoords_buffer);
}
