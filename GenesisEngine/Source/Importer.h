#pragma once

typedef unsigned int uint;

//Engine Classes
class ResourceModel;
struct ModelNode;
class ResourceMesh;
class ResourceShader;

//Assimp classes
class aiScene;
struct aiNode;
class aiMesh;
class aiMaterial;

namespace Importer
{
	//Model
	int ImportModel(char* fileBuffer, uint fileSize, ResourceModel* model);
	int ProcessAssimpNode(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model);
	
	//Mesh
	int ImportMesh(aiMesh* aimesh, ResourceMesh* mesh);

	//Shader
	int ImportShader(char* fileBuffer, ResourceShader* shader);
}

