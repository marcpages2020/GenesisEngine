#include "Importers.h"
#include "Timer.h"
#include "FileSystem.h"
#include "Application.h"

#include <unordered_set>

#include "Resource.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Mesh.h"
#include "GameObject.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "GnJSON.h"

#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"

#include "Devil/include/IL/il.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#pragma comment (lib, "Assimp/Assimp/libx86/assimp.lib")

#pragma comment (lib, "Devil/libx86/DevIL.lib")	
#pragma comment (lib, "Devil/libx86/ILU.lib")	
#pragma comment (lib, "Devil/libx86/ILUT.lib")	

#pragma region ModelImporter

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

		/*
		for (size_t i = 0; i < scene->mNumLights; i++)
		{
			Light light;
			aiLight* ai_light = scene->mLights[i];
			light.SetPos(ai_light->mPosition.x, ai_light->mPosition.y, ai_light->mPosition.z);
			light.ambient = Color(ai_light->mColorAmbient.r, ai_light->mColorAmbient.g, ai_light->mColorAmbient.b);
			light.diffuse = Color(ai_light->mColorDiffuse.r, ai_light->mColorDiffuse.g, ai_light->mColorDiffuse.b);
			model->lights.push_back(light);
		}

		for (size_t i = 0; i < scene->mNumCameras; i++)
		{
			Camera camera;
			aiCamera* aicamera = scene->mCameras[i];
			//TODO: set aspect ratio
			camera.SetHorizontalFieldOfView(aicamera->mHorizontalFOV);
			camera.SetPosition(float3(aicamera->mPosition.x, aicamera->mPosition.y, aicamera->mPosition.z));
			camera.SetNearPlaneDistance(aicamera->mClipPlaneNear);
			camera.SetFarPlaneDistance(aicamera->mClipPlaneFar);
			camera.SetReference(float3(aicamera->mLookAt.x, aicamera->mLookAt.y, aicamera->mLookAt.z));
		}
		*/

		aiNode* rootNode = scene->mRootNode;
		ImportChildren(scene, rootNode, nullptr, 0, model);
		ConvertToDesiredAxis(rootNode, model->nodes[0]);		

		aiReleaseImport(scene);

		LOG("%s: imported in %d ms", model->assetsFile.c_str(), timer.Read());
	}
	else
		LOG_ERROR("Error importing: %s", model->assetsFile.c_str());
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
			node_object.AddString("mesh_library_path", App->resources->GetLibraryPath(model->nodes[i].meshID));
		}

		if (model->nodes[i].materialID != -1)
		{
			node_object.AddInt("MaterialID", model->nodes[i].materialID);
			node_object.AddString("material_library_path", App->resources->GetLibraryPath(model->nodes[i].materialID));
		}

		nodes_array.AddObject(node_object);
	}

	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

void ModelImporter::ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model)
{
	ModelNode modelNode;

	if (node == scene->mRootNode)
		modelNode.name = FileSystem::GetFileName(model->assetsFile.c_str());
	else 
		modelNode.name = node->mName.C_Str();

	modelNode.UID = App->resources->GenerateUID();
	modelNode.parentUID = parentNodeUID;

	LoadTransform(node, modelNode);

	if (node->mMeshes != nullptr)
	{
		//Mesh --------------------------------------------------------------
		modelNode.meshID = model->meshes[*node->mMeshes];

		//Materials ----------------------------------------------------------
		aiMesh* aimesh = scene->mMeshes[*node->mMeshes];
		modelNode.materialID = model->materials[aimesh->mMaterialIndex];
	}

	model->nodes.push_back(modelNode);

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ImportChildren(scene, node->mChildren[i], node, modelNode.UID, model);
	}
}

void ModelImporter::ReimportFile(char* fileBuffer, ResourceModel* newModel, uint size)
{
	std::string meta_file = newModel->assetsFile;
	meta_file.append(".meta");

	ResourceModel oldModel(0);
	ExtractInternalResources(meta_file.c_str(), oldModel);

	Import(fileBuffer, newModel, size);

	for (size_t n = 0; n < newModel->nodes.size(); n++)
	{
		for (size_t o = 0; o < oldModel.nodes.size(); o++)
		{
			if (oldModel.nodes[o].name == newModel->nodes[n].name) 
			{
				newModel->nodes[n].meshID = oldModel.nodes[o].meshID;
				std::string mesh_path = App->resources->GenerateLibraryPath(oldModel.nodes[o].meshID, ResourceType::RESOURCE_MESH);
				if (FileSystem::Exists(mesh_path.c_str()))
				{
					std::string temp_path = App->resources->GenerateLibraryPath(newModel->nodes[n].meshID, ResourceType::RESOURCE_MESH);
					FileSystem::Rename(temp_path.c_str(), mesh_path.c_str());
				}

				newModel->nodes[n].materialID = oldModel.nodes[o].materialID;
				std::string material_path = App->resources->GenerateLibraryPath(oldModel.nodes[o].materialID, ResourceType::RESOURCE_MATERIAL);
				if (FileSystem::Exists(material_path.c_str()))
				{
					std::string temp_path = App->resources->GenerateLibraryPath(newModel->nodes[n].materialID, ResourceType::RESOURCE_MATERIAL);
					FileSystem::Rename(temp_path.c_str(), material_path.c_str());
				}
			}
		}
	}
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

	scaling *= App->resources->modelImportingOptions.globalScale;

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
			App->resources->CreateResourceData(modelNode.meshID, model->assetsFile.c_str(), nodeObject.GetString("mesh_library_path", "No Path"));
			meshes.emplace(modelNode.meshID);
			//if (App->resources->LoadResource(modelNode.meshID, ResourceType::RESOURCE_MESH) == nullptr)
				//ret = false;
		}

		modelNode.materialID = nodeObject.GetInt("MaterialID");
		if (modelNode.materialID != -1)
		{
			App->resources->CreateResourceData(modelNode.materialID, model->assetsFile.c_str(), nodeObject.GetString("material_library_path", "No Path"));
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

	App->resources->ReleaseResource(model->GetUID());

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

		int meshID = nodeObject.GetInt("Mesh UID");
		if (meshID != -1)
		{
			//avoid duplicating meshes
			if (std::find(meshes.begin(), meshes.end(), meshID) == meshes.end()) {
				meshes.push_back(meshID);
			}
		}

		int materialID = nodeObject.GetInt("Material UID");
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
		modelNode.meshID = nodeObject.GetInt("Mesh UID");
		modelNode.materialID = nodeObject.GetInt("Material UID");
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
				App->resources->CreateResourceData(meshID, assets_file.c_str(), meshLibraryPath.c_str());
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
				App->resources->CreateResourceData(materialID, assets_file.c_str(), materialLibraryPath.c_str());
			}
		}
	}

	RELEASE_ARRAY(buffer);

	return ret;
}

void ModelImporter::ConvertToDesiredAxis(aiNode* node, ModelNode& modelNode)
{
	if (node->mMetaData == nullptr)
		return;

	int upAxis;
	node->mMetaData->Get("UpAxis", upAxis);
	int upAxisSign;
	node->mMetaData->Get("UpAxisSign", upAxisSign);
	upAxis *= upAxisSign;

	int frontAxis;
	node->mMetaData->Get("frontAxis", frontAxis);
	int frontAxisSign;
	node->mMetaData->Get("frontAxisSign", frontAxisSign);
	frontAxis *= frontAxisSign;

	int coordAxis;
	node->mMetaData->Get("coordAxis", coordAxis);
	int coordAxisSign;
	node->mMetaData->Get("coordAxisSign", coordAxisSign);
	coordAxis *= coordAxisSign;

	float3x3 modelBasis = float3x3::zero;
	modelBasis[coordAxis][0] = coordAxisSign;
	modelBasis[upAxis][1] = upAxisSign;
	modelBasis[frontAxis][2] = frontAxisSign;

	float3x3 desiredBasis = float3x3::zero;

	float3 desiredForwardAxis = float3::zero;
	desiredForwardAxis[App->resources->modelImportingOptions.forwardAxis % 3] = App->resources->modelImportingOptions.forwardAxis;

	float3 desiredUpAxis = float3::zero;
	desiredUpAxis[App->resources->modelImportingOptions.upAxis % 3] = App->resources->modelImportingOptions.upAxis;

	float3 desiredCoordAxis = float3::zero;
	desiredCoordAxis = desiredForwardAxis.Cross(desiredUpAxis);

	desiredBasis.SetCol(0, desiredCoordAxis);
	desiredBasis.SetCol(1, desiredUpAxis);
	desiredBasis.SetCol(2, desiredForwardAxis);

	float3 rotation = modelBasis.ToEulerXYZ();

	modelNode.rotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
}

#pragma endregion

#pragma region MeshImporter

void MeshImporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void MeshImporter::CleanUp()
{
	aiDetachAllLogStreams();
}

void MeshImporter::Import(const aiMesh* aimesh, ResourceMesh* mesh)
{
	Timer timer;
	timer.Start();

	//vertex copying
	mesh->vertices_amount = aimesh->mNumVertices;
	mesh->vertices = new float[mesh->vertices_amount * 3]();
	memcpy(mesh->vertices, aimesh->mVertices, sizeof(float) * mesh->vertices_amount * 3);
	LOG("%s imported with %d vertices", aimesh->mName.C_Str(), mesh->vertices_amount);

	//indices copying
	if (aimesh->HasFaces())
	{
		mesh->indices_amount = aimesh->mNumFaces * 3;
		mesh->indices = new uint[mesh->indices_amount]();
		LOG("%s imported with %d indices", aimesh->mName.C_Str(), mesh->indices_amount);

		for (size_t f = 0; f < aimesh->mNumFaces; f++)
		{
			if (aimesh->mFaces[f].mNumIndices != 3)
			{
				LOG_WARNING("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&mesh->indices[f * 3], aimesh->mFaces[f].mIndices, 3 * sizeof(uint));
			}
		}
	}

	mesh->texcoords_amount = aimesh->mNumVertices;
	mesh->texcoords = new float[mesh->vertices_amount * 2]();
	mesh->colors = new float[mesh->indices_amount * 4]();

	if (aimesh->HasNormals())
	{
		mesh->normals_amount = aimesh->mNumVertices;
		mesh->normals = new float[aimesh->mNumVertices * 3]();
	}

	//normals, color and texture coordinates
	for (size_t v = 0, n = 0, tx = 0, c = 0; v < aimesh->mNumVertices; v++, n += 3, tx += 2, c += 4)
	{
		//normal copying
		if (aimesh->HasNormals())
		{
			//normal copying
			mesh->normals[n] = aimesh->mNormals[v].x;
			mesh->normals[n + 1] = aimesh->mNormals[v].y;
			mesh->normals[n + 2] = aimesh->mNormals[v].z;
		}

		//texcoords copying
		if (aimesh->mTextureCoords[0])
		{
			mesh->texcoords[tx] = aimesh->mTextureCoords[0][v].x;
			mesh->texcoords[tx + 1] = aimesh->mTextureCoords[0][v].y;
		}
		else
		{
			mesh->texcoords[tx] = 0.0f;
			mesh->texcoords[tx + 1] = 0.0f;
		}

		//color copying
		if (aimesh->HasVertexColors(v))
		{
			mesh->colors[c] = aimesh->mColors[v]->r;
			mesh->colors[c + 1] = aimesh->mColors[v]->g;
			mesh->colors[c + 2] = aimesh->mColors[v]->b;
			mesh->colors[c + 3] = aimesh->mColors[v]->a;
		}
		else
		{
			mesh->colors[c] = 0.0f;
			mesh->colors[c + 1] = 0.0f;
			mesh->colors[c + 2] = 0.0f;
			mesh->colors[c + 3] = 0.0f;
		}
	}

	LOG("Mesh imported in %d ms", timer.Read());
}

uint64 MeshImporter::Save(ResourceMesh* mesh, char** fileBuffer)
{
	uint ranges[4] = { mesh->indices_amount, mesh->vertices_amount, mesh->normals_amount, mesh->texcoords_amount };

	uint size = sizeof(ranges) + sizeof(uint) * mesh->indices_amount + sizeof(float) * mesh->vertices_amount * 3
		+ sizeof(float) * mesh->normals_amount * 3 + sizeof(float) * mesh->texcoords_amount * 2;

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//store indices
	bytes = sizeof(uint) * mesh->indices_amount;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//store vertices
	bytes = sizeof(float) * mesh->vertices_amount * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	//store normals
	bytes = sizeof(float) * mesh->normals_amount * 3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	//store texcoords
	bytes = sizeof(float) * mesh->texcoords_amount * 2;
	memcpy(cursor, mesh->texcoords, bytes);

	*fileBuffer = buffer;

	return size;
}

bool MeshImporter::Load(char* fileBuffer, ResourceMesh* mesh, uint size)
{
	bool ret = true;

	Timer timer;
	timer.Start();

	char* cursor = fileBuffer;

	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	mesh->indices_amount = ranges[0];
	mesh->vertices_amount = ranges[1];
	mesh->normals_amount = ranges[2];
	mesh->texcoords_amount = ranges[3];

	// Load indices
	bytes = sizeof(uint) * mesh->indices_amount;
	mesh->indices = new uint[mesh->indices_amount];
	memcpy(mesh->indices, cursor, bytes);
	cursor += bytes;

	//load vertices
	bytes = sizeof(float) * mesh->vertices_amount * 3;
	mesh->vertices = new float[mesh->vertices_amount * 3];
	memcpy(mesh->vertices, cursor, bytes);
	cursor += bytes;

	//load normals
	bytes = sizeof(float) * mesh->normals_amount * 3;
	mesh->normals = new float[mesh->normals_amount * 3];
	memcpy(mesh->normals, cursor, bytes);
	cursor += bytes;

	//load texcoords
	bytes = sizeof(float) * mesh->texcoords_amount * 2;
	mesh->texcoords = new float[mesh->texcoords_amount * 2];
	memcpy(mesh->texcoords, cursor, bytes);
	cursor += bytes;

	LOG("%s loaded in %d ms", mesh->libraryFile.c_str(), timer.Read());

	mesh->GenerateBuffers();

	return ret;
}

#pragma endregion 

#pragma region TextureImporter

void TextureImporter::Init()
{
	ilInit();
	iluInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL initted correctly");
}

void TextureImporter::Import(char* fileBuffer, ResourceTexture* texture, uint size)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error = IL_NO_ERROR;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	//ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		//LOG_ERROR("%s, %d %s", texture->assetsFile.c_str(), ilGetError(), iluErrorString(error));
		//return;
	}

	ILenum file_format = GetFileFormat(texture->assetsFile.c_str());

	if (ilLoadL(file_format, fileBuffer, size) == IL_FALSE)
	{
		LOG_ERROR("Error importing texture %s - %d: %s", texture->assetsFile, ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		LOG_ERROR("%d: %s", error, iluErrorString(error));
	}
	else
	{
		LOG("Texture loaded successfully from: %s in %d ms", texture->assetsFile.c_str(), timer.Read());

		texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}

	ilBindImage(0);
}

uint TextureImporter::Save(ResourceTexture* texture, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilBindImage(texture->GetID());

	TextureImportingOptions importingOptions = App->resources->textureImportingOptions;

	if (importingOptions.flip)
		iluFlipImage();

	ApplyImportingOptions(App->resources->textureImportingOptions);

	ilSetInteger(IL_DXTC_DATA_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0)
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
	}
	else
	{
		ILenum error;
		error = ilGetError();

		if (error != IL_NO_ERROR)
		{
			LOG_ERROR("Error when saving %s - %d: %s", texture->assetsFile, error, iluErrorString(error));
		}
	}

	ilBindImage(0);

	return size;
}

bool TextureImporter::Load(char* fileBuffer, ResourceTexture* texture, uint size)
{
	bool ret = true;
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	if (ilLoadL(IL_DDS, fileBuffer, size) == IL_FALSE)
	{
		LOG_ERROR("Error loading texture %s - %d: %s", texture->libraryFile.c_str(), ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return false;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		//ilBindImage(0);
		//ilDeleteImages(1, &imageID);
		LOG_ERROR("Error %d when loading %s: %s", error, texture->libraryFile.c_str(), iluErrorString(error));
		//ret = false;
	}
	else
	{
		LOG("Texture loaded successfully from: %s in %d ms", texture->libraryFile.c_str(), timer.Read());
		texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

		//LOG("Texture loaded successfully from: %s in %d ms", texture->libraryFile.c_str(), timer.Read());

		//texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}

	ilBindImage(0);
	return ret;
}

std::string TextureImporter::FindTexture(const char* texture_name, const char* model_directory)
{
	std::string path;
	FileSystem::SplitFilePath(model_directory, &path);
	std::string texture_path = path + texture_name;

	//Check if the texture is in the same folder
	if (FileSystem::Exists(texture_path.c_str()))
	{
		return texture_path.c_str();
	}
	else
	{
		//Check if the texture is in a sub folder
		texture_path = path + "Textures/" + texture_name;
		if (FileSystem::Exists(texture_path.c_str()))
		{
			return texture_path.c_str();
		}
		else
		{
			//Check if the texture is in the root textures folder
			texture_path = std::string("Assets/Textures/") + texture_name;
			if (FileSystem::Exists(texture_path.c_str()))
			{
				return texture_path.c_str();
			}
		}
	}

	texture_path.clear();
	return texture_path;
}

void TextureImporter::UnloadTexture(uint imageID)
{
	ilDeleteImages(1, &imageID);
}

ILenum TextureImporter::GetFileFormat(const char* file)
{
	ILenum file_format = IL_TYPE_UNKNOWN;
	std::string format = FileSystem::GetFileFormat(file);

	if (format == ".png")
		file_format = IL_PNG;
	else if (format == ".jpg")
		file_format = IL_JPG;
	else if (format == ".bmp")
		file_format = IL_BMP;

	return file_format;
}

void TextureImporter::ApplyImportingOptions(TextureImportingOptions importingOptions)
{
	if (importingOptions.flip)
		iluFlipImage();

	if (importingOptions.alienify)
		iluAlienify();

	if (importingOptions.blur_average)
		iluBlurAvg(10);

	if (importingOptions.blur_gaussian)
		iluBlurGaussian;

	if (importingOptions.equalize)
		iluEqualize();

	if (importingOptions.negativity)
		iluNegative();

	if (importingOptions.noise)
		iluNoisify(importingOptions.noise_tolerance);

	if (importingOptions.pixelize)
		iluPixelize(importingOptions.pixelize_size);

	if (importingOptions.sharpening) 
		iluSharpen(importingOptions.sharpening_factor, importingOptions.sharpening_iterations);

	iluGammaCorrect(importingOptions.gamma_correction);

	bool pixelize = false;
	bool sharpening = false;
	float contrast = 1.0f;
}

#pragma endregion

#pragma region MaterialImporter

void MaterialImporter::Import(const aiMaterial* aimaterial, ResourceMaterial* material)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	aiColor3D aiDiffuseColor;
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);

	if (path.length > 0)
	{
		std::string file_path = material->assetsFile;
		path = FileSystem::GetFile(path.C_Str());
		file_path = TextureImporter::FindTexture(path.C_Str(), material->assetsFile.c_str());

		if(file_path.size() > 0)
		{
			material->diffuseTextureUID = App->resources->ImportFile(file_path.c_str());
		}

		material->diffuseColor.r = aiDiffuseColor.r;
		material->diffuseColor.g = aiDiffuseColor.g;
		material->diffuseColor.b = aiDiffuseColor.b;

		//LOG("%s imported in %.3f s", texture->path.c_str(), timer.ReadSec());
	}
}

uint64 MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	GnJSONObj base_object;
	base_object.AddInt("Diffuse Texture", material->diffuseTextureUID);

	base_object.AddColor("diffuseColor", material->diffuseColor);

	char* buffer;
	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

bool MaterialImporter::Load(const char* fileBuffer, ResourceMaterial* material, uint size)
{
	bool ret = true;
	Timer timer;
	timer.Start();

	GnJSONObj material_data(fileBuffer);
	material->diffuseTextureUID = material_data.GetInt("Diffuse Texture");


	if(material->diffuseTextureUID != 0)
		App->resources->LoadResource(material->diffuseTextureUID, ResourceType::RESOURCE_TEXTURE);

	material->diffuseColor = material_data.GetColor("diffuseColor");

	material_data.Release();
	return ret;
}

bool MaterialImporter::Unload(uint imageID)
{
	bool ret = true;

	ilDeleteImages(1, &imageID);

	return ret;
}

bool MaterialImporter::DeleteTexture(const char* material_library_path)
{
	bool ret = true;

	char* buffer = nullptr;
	FileSystem::Load(material_library_path, &buffer);

	GnJSONObj material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("Diffuse Texture");
	const char* texture_library_path = App->resources->Find(diffuseTextureUID);

	if (texture_library_path != nullptr)
	{
		FileSystem::Delete(texture_library_path);
		App->resources->ReleaseResource(diffuseTextureUID);
		App->resources->ReleaseResourceData(diffuseTextureUID);
	}
	else
		LOG_WARNING("Texture: %s could not be deleted. Not found", material_library_path);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

const char* MaterialImporter::ExtractTexture(const char* material_library_path)
{
	char* buffer = nullptr;
	FileSystem::Load(material_library_path, &buffer);

	GnJSONObj material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("Diffuse Texture");
	const char* texture_library_path = App->resources->Find(diffuseTextureUID);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return texture_library_path;
}

#pragma endregion