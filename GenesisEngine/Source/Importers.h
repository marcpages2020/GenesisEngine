#pragma once

#include "Globals.h"
#include <vector>
#include <string>

#pragma region ForwardDeclarations

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

#pragma endregion

namespace ModelImporter
{
	GameObject* Import(const char* path);
	GameObject* ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, GameObject* parentGameObject, const char* path, GnJSONArray& mesh_array);
	void LoadTransform(aiNode* node, Transform* transform);
}

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, GnMesh* mesh);
	uint64 Save(GnMesh* mesh, char** fileBuffer);
	void Load(const char* fileBuffer, GnMesh* mesh);
}

namespace TextureImporter
{
	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
}

namespace MaterialImporter
{
	void Init();
	void Import(const aiMaterial* material, Material* ourMaterial, const char* folder_path);
	uint64 Save(Material* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, Material* material);

	GnTexture* LoadTexture(const char* path);
}