#pragma once

typedef unsigned int uint;

class ResourceModel;
struct ModelNode;
class ResourceMesh;

class aiScene;
struct aiNode;
class aiMesh;
class aiMaterial;

namespace Importer
{
	int ImportModel(char* fileBuffer, uint fileSize, ResourceModel* model);
	int ProcessAssimpNode(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model);
	int ImportMesh(aiMesh* aimesh, ResourceMesh* mesh);

	//uint32 Load(char* fileBuffer, ResourceModel*);
}

