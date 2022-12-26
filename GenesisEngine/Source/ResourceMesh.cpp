#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(uint newUid) : Resource(newUid, ResourceType::MESH), name("None"), numVertices(0), vertices(nullptr)
{}

ResourceMesh::~ResourceMesh()
{
	delete name;
	numVertices = 0;
	delete vertices;
}
