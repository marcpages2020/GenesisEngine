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
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model);
	void ReimportFile(char* fileBuffer, ResourceModel* resource, uint size);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	bool Load(char* fileBuffer, ResourceModel* model, uint size);
	bool DrawImportingWindow(const char* file_to_import, ModelImportingOptions& importingOptions);

	GameObject* ConvertToGameObject(ResourceModel* model);
	void ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ExtractInternalResources(const char* meta_file, ResourceModel& model);
	bool InternalResourcesExist(const char* path);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
}

namespace MeshImporter
{
	void Init();
	void CleanUp();

	void Import(const aiMesh* aimesh, ResourceMesh* mesh);
	uint64 Save(ResourceMesh* mesh, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceMesh* mesh, uint size);
}

namespace TextureImporter
{
	void Init();

	void Import(char* fileBuffer, ResourceTexture* resource, uint size);
	uint Save(ResourceTexture* texture, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceTexture* texture, uint size);
	bool DrawImportingWindow(const char* file_to_import, TextureImportingOptions& importingOptions);

	std::string FindTexture(const char* texture_name, const char* model_directory);
	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
}

namespace MaterialImporter
{
	void Import(const aiMaterial* aimaterial, ResourceMaterial* material);
	uint64 Save(ResourceMaterial* material, char** fileBuffer);
	bool Load(const char* fileBuffer, ResourceMaterial* material, uint size);
	bool Unload(uint imageID);

	bool DeleteTexture(const char* material_library_path);
	const char* ExtractTexture(const char* material_library_path);
}