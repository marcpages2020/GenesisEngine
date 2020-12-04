#pragma once

#include "Globals.h"
#include "ImportingOptions.h"
#include <vector>
#include <string>

#pragma region ForwardDeclarations

class Resource;
class ResourceModel;
class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;

struct ModelNode;
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
	void Init();
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	void Load(const char* path, ResourceModel* model);
	bool DrawImportingWindow(const char* file_to_import, ModelImportingOptions& importingOptions);

	GameObject* ConvertToGameObject(ResourceModel* model);
	void ExtractInternalResources(const char* library_path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
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
	bool DrawImportingWindow(const char* file_to_import, TextureImportingOptions& importingOptions);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
}

namespace MaterialImporter
{
	void Import(const aiMaterial* aimaterial, ResourceMaterial* material);
	uint64 Save(ResourceMaterial* material, char** fileBuffer);
	void Load(const char* file_path, ResourceMaterial* material);

	bool DeleteTexture(const char* material_library_path);
	const char* ExtractTexture(const char* material_library_path);
}