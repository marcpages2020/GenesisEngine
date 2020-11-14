#pragma once

#include "Globals.h"
#include <vector>
#include <string>

#pragma region ForwardDeclarations

class Resource;
class ResourceMesh;
class ResourceTexture;

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
	void Import(char* fileBuffer, Resource* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, const char* path, GnJSONArray& meshes_array);
	void LoadTransform(aiNode* node, Transform* transform);
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

	void Import(char* fileBuffer, Resource* resource, uint size);
	uint Save(char** fileBuffer, ResourceTexture* texture);
	void Load(const char* path);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
}

namespace MaterialImporter
{
	void Import(const aiMaterial* material, Material* ourMaterial, const char* folder_path);
	uint64 Save(Material* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, Material* material);

	GnTexture* LoadTexture(const char* path);
}