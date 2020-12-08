#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

#include "Globals.h"
#include "ImportingOptions.h"
#include <vector>
#include <string>

class Resource;
class ResourceModel;
class ResourceMesh;

struct ModelNode;
class GnJSONArray;
class GameObject;
struct GnTexture;
class GnMesh;
class Transform;
class Material;

class aiMesh;
class aiScene;
struct aiNode;
class aiMaterial;

#include "MathGeoLib/include/MathGeoLib.h"

namespace ModelImporter
{
	void Import(char* fileBuffer, ResourceModel* resource, uint size);
	void ImportChildren(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model);
	void AddParentTransform(ModelNode* node, ModelNode* parentNode);
	void LoadTransform(aiNode* node, ModelNode& modelNode);
	void ReimportFile(char* fileBuffer, ResourceModel* resource, uint size);
	uint64 Save(ResourceModel* model, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceModel* model, uint size);

	GameObject* ConvertToGameObject(ResourceModel* model);
	void ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials);
	void ExtractInternalResources(const char* meta_file, ResourceModel& model);
	bool InternalResourcesExist(const char* path);
	void ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode);
}

#endif // !_MODEL_IMPORTER_H_
