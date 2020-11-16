#pragma once

#include "Globals.h"
#include <vector>
#include <string>

#pragma region ForwardDeclarations

class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;

class GnJSONObj;
class GnJSONArray;
class GameObject;
struct GnTexture;
class aiMesh;
class GnMesh;
class Transform;
class Material;

class aiScene;
struct aiNode;
class aiMaterial;

typedef unsigned int ILenum;

#pragma endregion

namespace ModelImporter
{
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	void ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model, GnJSONArray& meshes_array);
	void LoadTransform(aiNode* node, GnJSONObj& node_object);
	GameObject* Load(const char* path, ResourceModel* model);
}

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	void Load(const char* fileBuffer, ResourceMesh* mesh);
}

namespace TextureImporter
{
	void Init();

	void Import(char* fileBuffer, ResourceTexture* resource, uint size);
	uint Save(ResourceTexture* texture, char** fileBuffer);
	void Load(const char* path, ResourceTexture* texture);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
}

namespace MaterialImporter
{
	void Import(const aiMaterial* aimaterial, ResourceMaterial* material);
	uint64 Save(ResourceMaterial* material, char** fileBuffer);
	void Load(const char* file_path, ResourceMaterial* material);
}