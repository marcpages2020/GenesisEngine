#ifndef _MESH_IMPORTER_H_
#define _MESH_IMPORTER_H_
#include "Globals.h"

class ResourceMesh;

class aiMesh;

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMesh* mesh, uint size);
}

#endif // !_MESH_IMPORTER_H_