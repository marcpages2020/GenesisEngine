#include "ResourceMesh.h"

ResourceMesh::ResourceMesh(uint UID) : Resource(UID, ResourceType::RESOURCE_MESH),
 vertices_amount(-1), vertices(nullptr), 
 indices_amount(-1), indices(nullptr),
 normals_amount(-1), normals(nullptr), 
 texcoords(nullptr), colors(nullptr) {}

ResourceMesh::~ResourceMesh(){}
