#include "ModelImporter.h"
#include "Application.h"
#include "FileSystem.h"
#include <unordered_set>

#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

#include "ResourceModel.h"

#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/Assimp/libx86/assimp.lib")

void ModelImporter::Import(char* fileBuffer, ResourceModel* model, uint size)
{
	Timer timer;
	timer.Start();

	const aiScene* scene = nullptr;
	scene = aiImportFileFromMemory(fileBuffer, size, aiProcessPreset_TargetRealtime_MaxQuality, NULL);

	if (scene != nullptr && scene->HasMeshes())
	{
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[i];
			model->meshes.push_back(App->resources->ImportInternalResource(model->assetsFile.c_str(), aimesh, ResourceType::RESOURCE_MESH));
		}

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aimaterial = scene->mMaterials[i];
			model->materials.push_back(App->resources->ImportInternalResource(model->assetsFile.c_str(), aimaterial, ResourceType::RESOURCE_MATERIAL));
		}

		if (!App->resources->modelImportingOptions.ignoreLights)
		{
			for (size_t i = 0; i < scene->mNumLights; i++)
			{
				Light* light = new Light();
				aiLight* ai_light = scene->mLights[i];
				light->name = ai_light->mName.C_Str();
				light->SetPos(ai_light->mPosition.x, ai_light->mPosition.y, ai_light->mPosition.z);
				light->ambient = Color(ai_light->mColorAmbient.r, ai_light->mColorAmbient.g, ai_light->mColorAmbient.b);
				light->diffuse = Color(ai_light->mColorDiffuse.r, ai_light->mColorDiffuse.g, ai_light->mColorDiffuse.b);
				model->lights.push_back(light);
			}
		}

		if (!App->resources->modelImportingOptions.ignoreCameras)
		{
			for (size_t i = 0; i < scene->mNumCameras; i++)
			{
				Camera* camera = new Camera;
				aiCamera* aicamera = scene->mCameras[i];
				camera->name = aicamera->mName.C_Str();
				camera->SetHorizontalFieldOfView(aicamera->mHorizontalFOV);
				camera->SetPosition(float3(aicamera->mPosition.x, aicamera->mPosition.y, aicamera->mPosition.z));
				camera->SetNearPlaneDistance(aicamera->mClipPlaneNear);
				camera->SetFarPlaneDistance(aicamera->mClipPlaneFar);
				camera->SetReference(float3(aicamera->mLookAt.x, aicamera->mLookAt.y, aicamera->mLookAt.z));
				model->cameras.push_back(camera);
			}
		}

		aiNode* rootNode = scene->mRootNode;
		ImportChildren(scene, rootNode, nullptr, nullptr, model);

		aiReleaseImport(scene);

		LOG("%s: imported in %d ms", model->assetsFile.c_str(), timer.Read());
	}
	else
		LOG_ERROR("Error importing: %s", model->assetsFile.c_str());
}

void ModelImporter::ImportChildren(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model)
{
	ModelNode modelNode;

	LoadTransform(ainode, modelNode);

	if (ainode == scene->mRootNode)
	{
		modelNode.name = FileSystem::GetFileName(model->assetsFile.c_str());
		modelNode.parentUID = 0;
		ConvertToDesiredAxis(ainode, modelNode);
		modelNode.scale *= App->resources->modelImportingOptions.globalScale;
	}
	else
	{
		modelNode.name = ainode->mName.C_Str();
		modelNode.parentUID = parentNode->UID;
	}

	modelNode.UID = App->resources->GenerateUID();

	if (modelNode.name.find("_$AssimpFbx$_") != std::string::npos && ainode->mNumChildren == 1)
	{
		modelNode.UID = parentNode->UID;
		modelNode.parentUID = parentNode->parentUID;
		AddParentTransform(&modelNode, parentNode);
	}
	else
	{
		if (parentNode != nullptr && parentNode->name.find("_$AssimpFbx$_") != std::string::npos)
		{
			AddParentTransform(&modelNode, parentNode);
		}

		if (ainode->mMeshes != nullptr)
		{
			//Mesh --------------------------------------------------------------
			modelNode.meshID = model->meshes[*ainode->mMeshes];

			//Materials ----------------------------------------------------------
			aiMesh* aimesh = scene->mMeshes[*ainode->mMeshes];
			modelNode.materialID = model->materials[aimesh->mMaterialIndex];
		}

		
		if(modelNode.name.find(".Target") == std::string::npos)
			model->nodes.push_back(modelNode);
		else
		{
			for (size_t i = 0; i < model->cameras.size(); i++)
			{
				if (modelNode.name.find(model->cameras[i]->name) != std::string::npos) 
				{
					model->cameras[i]->LookAt(modelNode.position);
				}
			}
			//model->nodes.push_back(modelNode);
		}
	}

	for (size_t i = 0; i < ainode->mNumChildren; i++)
	{
		ImportChildren(scene, ainode->mChildren[i], ainode, &modelNode, model);
	}
}

uint64 ModelImporter::Save(ResourceModel* model, char** fileBuffer)
{
	char* buffer;

	GnJSONObj base_object;
	GnJSONArray nodes_array = base_object.AddArray("Nodes");

	for (size_t i = 0; i < model->nodes.size(); i++)
	{
		GnJSONObj node_object;

		node_object.AddString("Name", model->nodes[i].name.c_str());
		node_object.AddInt("UID", model->nodes[i].UID);
		node_object.AddInt("Parent UID", model->nodes[i].parentUID);

		node_object.AddFloat3("Position", model->nodes[i].position);
		node_object.AddQuaternion("Rotation", model->nodes[i].rotation);
		node_object.AddFloat3("Scale", model->nodes[i].scale);

		if (model->nodes[i].meshID != -1)
		{
			node_object.AddInt("MeshID", model->nodes[i].meshID);
			node_object.AddString("mesh_library_path", App->resources->GenerateLibraryPath(model->nodes[i].meshID, ResourceType::RESOURCE_MESH).c_str());
		}

		if (model->nodes[i].materialID != -1)
		{
			node_object.AddInt("MaterialID", model->nodes[i].materialID);
			node_object.AddString("material_library_path", App->resources->GenerateLibraryPath(model->nodes[i].materialID, ResourceType::RESOURCE_MATERIAL).c_str());
		}

		nodes_array.AddObject(node_object);
	}

	//Lights --------------------------------------------------------
	GnJSONArray lights_array = base_object.AddArray("Lights");
	for (size_t i = 0; i < model->lights.size(); i++)
	{
		model->lights[i]->Save(lights_array);
	}

	//Cameras --------------------------------------------------------
	GnJSONArray cameras_array = base_object.AddArray("Cameras");
	for (size_t i = 0; i < model->cameras.size(); i++)
	{
		model->cameras[i]->Save(cameras_array);
	}

	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

void ModelImporter::AddParentTransform(ModelNode* node, ModelNode* parentNode)
{
	node->position += parentNode->position;
	node->rotation = node->rotation.Mul(parentNode->rotation);
	node->scale = node->scale.Mul(parentNode->scale);
}

void ModelImporter::ReimportFile(char* fileBuffer, ResourceModel* newModel, uint size)
{
	std::string meta_file = newModel->assetsFile;
	meta_file.append(".meta");

	ResourceModel oldModel(App->resources->GetUIDFromMeta(meta_file.c_str()));
	ExtractInternalResources(meta_file.c_str(), oldModel);

	Import(fileBuffer, newModel, size);

	for (size_t n = 0; n < newModel->nodes.size(); n++)
	{
		for (size_t o = 0; o < oldModel.nodes.size(); o++)
		{
			if (oldModel.nodes[o].name == newModel->nodes[n].name)
			{
				if (oldModel.nodes[o].meshID != -1) 
				{
					std::string oldModelPath = App->resources->GenerateLibraryPath(oldModel.nodes[o].meshID, ResourceType::RESOURCE_MESH);
					std::string newModelPath = App->resources->GenerateLibraryPath(newModel->nodes[n].meshID, ResourceType::RESOURCE_MESH);
					newModel->nodes[n].meshID = oldModel.nodes[o].meshID;
					
					if (FileSystem::Exists(newModelPath.c_str()))
						FileSystem::Rename(newModelPath.c_str(), oldModelPath.c_str());
				}

				if (oldModel.nodes[o].materialID != -1)
				{
					std::string oldModelPath = App->resources->GenerateLibraryPath(oldModel.nodes[o].materialID, ResourceType::RESOURCE_MATERIAL);
					std::string newModelPath = App->resources->GenerateLibraryPath(newModel->nodes[n].materialID, ResourceType::RESOURCE_MATERIAL);
					newModel->nodes[n].materialID = oldModel.nodes[o].materialID;
					
					if(FileSystem::Exists(newModelPath.c_str()))
						FileSystem::Rename(newModelPath.c_str(), oldModelPath.c_str());
				}

				break;
			}
		}
	}

	newModel->SetUID(oldModel.GetUID());
}

void ModelImporter::LoadTransform(aiNode* node, ModelNode& modelNode)
{
	aiVector3D position, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, position);
	//eulerRotation = rotation.GetEuler() * RADTODEG;

	if (App->resources->modelImportingOptions.normalizeScales && scaling.x == 100 && scaling.y == 100 && scaling.z == 100) {
		scaling.x = scaling.y = scaling.z = 1.0f;
	}

	//scaling *= App->resources->modelImportingOptions.globalScale;

	modelNode.position = float3(position.x, position.y, position.z);
	modelNode.rotation = Quat(rotation.x, rotation.y, rotation.z, rotation.w);
	modelNode.scale = float3(scaling.x, scaling.y, scaling.z);
}

bool ModelImporter::Load(char* fileBuffer, ResourceModel* model, uint size)
{
	bool ret = true;

	GnJSONObj model_data(fileBuffer);
	GnJSONArray nodes_array = model_data.GetArray("Nodes");

	std::unordered_set<uint> meshes;
	std::unordered_set<uint> materials;

	for (size_t i = 0; i < nodes_array.Size(); i++)
	{
		GnJSONObj nodeObject = nodes_array.GetObjectAt(i);
		ModelNode modelNode;
		modelNode.name = nodeObject.GetString("Name", "No Name");
		modelNode.UID = nodeObject.GetInt("UID");
		modelNode.parentUID = nodeObject.GetInt("Parent UID");

		modelNode.position = nodeObject.GetFloat3("Position", float3::zero);
		modelNode.rotation = nodeObject.GetQuaternion("Rotation", Quat::identity);
		modelNode.scale = nodeObject.GetFloat3("Scale", float3::zero);

		modelNode.meshID = nodeObject.GetInt("MeshID");
		if (modelNode.meshID != -1)
		{
			App->resources->CreateResourceData(modelNode.meshID, modelNode.name.c_str(),
				model->assetsFile.c_str(), nodeObject.GetString("mesh_library_path", "No Path"));
			meshes.emplace(modelNode.meshID);
			//if (App->resources->LoadResource(modelNode.meshID, ResourceType::RESOURCE_MESH) == nullptr)
				//ret = false;
		}

		modelNode.materialID = nodeObject.GetInt("MaterialID");
		if (modelNode.materialID != -1)
		{
			App->resources->CreateResourceData(modelNode.materialID, modelNode.name.c_str(),
				model->assetsFile.c_str(), nodeObject.GetString("material_library_path", "No Path"));
			materials.emplace(modelNode.materialID);

			//if (App->resources->LoadResource(modelNode.materialID, ResourceType::RESOURCE_MATERIAL) == nullptr)
				//ret = false;
		}

		model->nodes.push_back(modelNode);
	}

	for (auto it = meshes.begin(); it != meshes.end(); ++it) {
		if (App->resources->LoadResource(*it, ResourceType::RESOURCE_MESH) == nullptr)
			ret = false;
	}

	for (auto it = materials.begin(); it != materials.end(); ++it) {
		if (App->resources->LoadResource(*it, ResourceType::RESOURCE_MATERIAL) == nullptr)
			ret = false;
	}

	GnJSONArray lights_array = model_data.GetArray("Lights");
	for (size_t i = 0; i < lights_array.Size(); i++)
	{
		GnJSONObj light_object = lights_array.GetObjectAt(i);
		Light* light = new Light();
		light->Load(light_object);
		model->lights.push_back(light);
	}

	GnJSONArray cameras_array = model_data.GetArray("Cameras");
	for (size_t i = 0; i < cameras_array.Size(); i++)
	{
		GnJSONObj cameras_object = cameras_array.GetObjectAt(i);
		Camera* camera = new Camera();
		camera->Load(cameras_object);
		camera->LookAt(camera->GetPosition() + camera->GetReference());
		model->cameras.push_back(camera);
	}

	model_data.Release();
	return ret;
}

GameObject* ModelImporter::ConvertToGameObject(ResourceModel* model)
{
	if (model == nullptr) {
		LOG_ERROR("Trying to load null model");
		return nullptr;
	}

	std::vector<GameObject*> createdGameObjects;

	GameObject* root = nullptr;

	for (size_t i = 0; i < model->nodes.size(); i++)
	{
		GameObject* gameObject = new GameObject();
		gameObject->SetName(model->nodes[i].name.c_str());
		gameObject->UUID = model->nodes[i].UID;
		gameObject->GetTransform()->SetPosition(model->nodes[i].position);
		gameObject->GetTransform()->SetRotation(model->nodes[i].rotation);
		gameObject->GetTransform()->SetScale(model->nodes[i].scale);

		if (model->nodes[i].meshID != -1)
		{
			GnMesh* mesh = (GnMesh*)gameObject->AddComponent(ComponentType::MESH);
			mesh->SetResourceUID(model->nodes[i].meshID);
		}

		if (model->nodes[i].materialID != -1)
		{
			Material* material = (Material*)gameObject->AddComponent(ComponentType::MATERIAL);
			material->SetResourceUID(model->nodes[i].materialID);
		}

		if (model->nodes[i].parentUID == 0)
			root = gameObject;

		for (size_t j = 0; j < createdGameObjects.size(); j++)
		{
			if (createdGameObjects[j]->UUID == model->nodes[i].parentUID)
			{
				createdGameObjects[j]->AddChild(gameObject);
				gameObject->SetParent(createdGameObjects[j]);
			}
		}

		createdGameObjects.push_back(gameObject);
	}

	for (size_t i = 0; i < model->lights.size(); i++) {
		for (size_t j = 0; j < createdGameObjects.size(); j++)
		{
			if (model->lights[i]->name.compare(createdGameObjects[j]->GetName()) == 0)
			{
				createdGameObjects[j]->SetName(model->lights[i]->name.c_str());
				createdGameObjects[j]->AddComponent(model->lights[i]);
			}
		}
	}

	for (size_t i = 0; i < model->cameras.size(); i++) 
	{
		for (size_t j = 0; j < createdGameObjects.size(); j++)
		{
			if (model->cameras[i]->name.compare(createdGameObjects[j]->GetName()) == 0) 
			{
				createdGameObjects[j]->SetName(model->cameras[i]->name.c_str());
				createdGameObjects[j]->AddComponent(model->cameras[i]);
			}
		}
	}

	App->resources->ReleaseResource(model->GetUID());
	root->UpdateChildrenTransforms();

	return root;
}

void ModelImporter::ExtractInternalResources(const char* path, std::vector<uint>& meshes, std::vector<uint>& materials)
{
	char* buffer = nullptr;
	uint size = FileSystem::Load(path, &buffer);
	GnJSONObj model_data(buffer);
	GnJSONArray nodes_array = model_data.GetArray("Nodes");

	for (size_t i = 0; i < nodes_array.Size(); i++)
	{
		GnJSONObj nodeObject = nodes_array.GetObjectAt(i);

		int meshID = nodeObject.GetInt("MeshID");
		if (meshID != -1)
		{
			//avoid duplicating meshes
			if (std::find(meshes.begin(), meshes.end(), meshID) == meshes.end()) {
				meshes.push_back(meshID);
			}
		}

		int materialID = nodeObject.GetInt("MaterialID");
		if (materialID != -1)
		{
			//avoid duplicating materials
			if (std::find(materials.begin(), materials.end(), materialID) == materials.end()) {
				materials.push_back(materialID);
			}
		}
	}

	model_data.Release();
	RELEASE_ARRAY(buffer);
}

void ModelImporter::ExtractInternalResources(const char* meta_file, ResourceModel& model)
{
	char* buffer = nullptr;
	uint size = FileSystem::Load(meta_file, &buffer);
	GnJSONObj model_data(buffer);
	GnJSONArray nodes_array = model_data.GetArray("Nodes");

	for (size_t i = 0; i < nodes_array.Size(); i++)
	{
		GnJSONObj nodeObject = nodes_array.GetObjectAt(i);
		ModelNode modelNode;

		modelNode.name = nodeObject.GetString("Name", "No Name");
		modelNode.meshID = nodeObject.GetInt("MeshID");
		modelNode.materialID = nodeObject.GetInt("MaterialID");

		model.nodes.push_back(modelNode);
	}

	model_data.Release();
	RELEASE_ARRAY(buffer);
}

bool ModelImporter::InternalResourcesExist(const char* path)
{
	bool ret = true;

	char* buffer;
	FileSystem::Load(path, &buffer);

	GnJSONObj meta_data(buffer);
	GnJSONArray nodes_array = meta_data.GetArray("Nodes");

	//generate an assets file if the original file is a meta file
	std::string assets_file = path;
	size_t size = assets_file.find(".meta");
	if (size != std::string::npos)
		assets_file = assets_file.substr(0, size);

	for (size_t i = 0; i < nodes_array.Size(); i++)
	{
		GnJSONObj nodeObject = nodes_array.GetObjectAt(i);
		std::string nodeName = nodeObject.GetString("Name", "No Name");

		int meshID = nodeObject.GetInt("MeshID");
		if (meshID != -1)
		{
			std::string meshLibraryPath = nodeObject.GetString("mesh_library_path", "No path");

			if (!FileSystem::Exists(meshLibraryPath.c_str())) {
				ret = false;
				LOG_ERROR("Mesh: %d not found", meshID);
				break;
			}
			else
			{
				App->resources->CreateResourceData(meshID, nodeName.c_str(), assets_file.c_str(), meshLibraryPath.c_str());
			}
		}

		int materialID = nodeObject.GetInt("MaterialID");
		if (materialID != -1)
		{
			std::string materialLibraryPath = nodeObject.GetString("material_library_path", "No path");

			if (!FileSystem::Exists(materialLibraryPath.c_str())) {
				ret = false;
				LOG_ERROR("Material: %s not found", materialLibraryPath.c_str());
				break;
			}
			else
			{
				App->resources->CreateResourceData(materialID, nodeName.c_str(), assets_file.c_str(), materialLibraryPath.c_str());
			}
		}
	}

	RELEASE_ARRAY(buffer);

	return ret;
}

void ModelImporter::ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode)
{
	ModelImportingOptions importingOptions = App->resources->modelImportingOptions;
	Axis upAxisEnum = importingOptions.upAxis;
	Axis forwardAxisEnum = importingOptions.forwardAxis;

	int forwardAxisSign = forwardAxisEnum < 3 ? 1 : -1;
	int upAxisSign = upAxisEnum < 3 ? 1 : -1;

	float3 upAxis = float3::zero;
	upAxis.At(upAxisEnum % 3) = upAxisSign;

	float3 forwardAxis = float3::zero;
	forwardAxis.At(forwardAxisEnum % 3) = forwardAxisSign;

	float3 rightAxis = forwardAxis.Cross(upAxis);

	float3x3 desiredAxis(rightAxis, upAxis, forwardAxis);

	float4x4 nodeTransformation = float4x4::FromTRS(modelNode.position, modelNode.rotation, modelNode.scale);
	nodeTransformation = desiredAxis * nodeTransformation;

	nodeTransformation.Decompose(modelNode.position, modelNode.rotation, modelNode.scale);
}