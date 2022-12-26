#pragma once

typedef unsigned int uint32;

class ResourceModel;
class ResourceMesh;

struct aiMesh;

namespace Importer
{
	int ImportModel(char* filePath, ResourceModel* model);
	int ImportMesh(aiMesh* aimesh, ResourceMesh* mesh);


	//uint32 Load(char* fileBuffer, ResourceModel*);
}

