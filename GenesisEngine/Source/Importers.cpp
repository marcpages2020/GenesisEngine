#include "Importers.h"
#include "Timer.h"
#include "FileSystem.h"
#include "Application.h"

#include "Resource.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Mesh.h"
#include "GameObject.h"
#include "Material.h"
#include "Transform.h"
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

		GnJSONObj save_file;
		GnJSONArray nodes = save_file.AddArray("Nodes");
		aiNode* rootNode = scene->mRootNode;
		ImportChildren(scene, rootNode, nullptr, 0, model, nodes);

		model->model_information = save_file;

		aiReleaseImport(scene);

		LOG("%s loaded in %d ms", model->assetsFile.c_str(), timer.Read());
	}
	else
		LOG_ERROR("Error loading scene %s", model->assetsFile.c_str());
}

uint64 ModelImporter::Save(ResourceModel* model, char** fileBuffer)
{
	GnJSONObj base_object; 

	char* buffer;
	uint size = model->model_information.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

void ModelImporter::ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, ResourceModel* model, GnJSONArray& meshes_array)
{
	GnJSONObj node_object;

	//Node Information 
	meshes_array.AddObject(node_object);
	node_object.AddString("Name", node->mName.C_Str());
	uint node_uid = App->resources->GenerateUID();
	node_object.AddInt("UUID", node_uid);
	node_object.AddInt("Parent UUID", parentNodeUID);

	LoadTransform(node, node_object);

	if (node->mMeshes != nullptr)
	{
		//Mesh --------------------------------------------------------------
		node_object.AddInt("Mesh", model->meshes[*node->mMeshes]);

		//Materials ----------------------------------------------------------
		aiMesh* aimesh = scene->mMeshes[*node->mMeshes];
		node_object.AddInt("Material", model->materials[aimesh->mMaterialIndex]);

	}

	meshes_array.AddObject(node_object);

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ImportChildren(scene, node->mChildren[i], node, node_uid, model, meshes_array);
	}
}

void ModelImporter::LoadTransform(aiNode* node, GnJSONObj& node_object)
{
	aiVector3D position, scaling, eulerRotation;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, position);
	//eulerRotation = rotation.GetEuler() * RADTODEG;

	if (FileSystem::normalize_scales && scaling.x == 100 && scaling.y == 100 && scaling.z == 100) {
		scaling.x = scaling.y = scaling.z = 1.0f;
	}

	node_object.AddFloat3("Position", float3(position.x, position.y, position.z));
	node_object.AddQuaternion("Rotation", Quat(rotation.x, rotation.y, rotation.z, rotation.w));
	node_object.AddFloat3("Scale", float3(scaling.x, scaling.y, scaling.z));
}

GameObject* ModelImporter::Load(const char* path, ResourceModel* model)
{
	//Load JSON and transform into Game Objects
	char* buffer = nullptr;
	uint size = FileSystem::Load(model->libraryFile.c_str(), &buffer);
	GnJSONObj model_data(buffer);
	GnJSONArray nodes = model_data.GetArray("Nodes");

	for (size_t i = 0; i < nodes.Size(); i++)
	{
		GnJSONObj node = nodes.GetObjectAt(i);
		model->meshes.push_back(node.GetInt("Mesh"));
	}

	GameObject* root = nullptr;
	std::vector<GameObject*> createdObjects;

	for (size_t i = 0; i < nodes.Size(); i++)
	{
		GnJSONObj node_data = nodes.GetObjectAt(i);

		int meshID = node_data.GetInt("Mesh", -1);
		if (meshID != -1)
		{
			App->resources->CreateResourceData(meshID, ResourceType::RESOURCE_MESH, model->assetsFile.c_str());
			App->resources->LoadResource(meshID);
		}

		int materialID = node_data.GetInt("Material", -1);
		if (materialID != -1)
		{
			App->resources->CreateResourceData(materialID, ResourceType::RESOURCE_MATERIAL, model->assetsFile.c_str());
			App->resources->LoadResource(materialID);
		}

		//load game object
		GameObject* gameObject = new GameObject();
		uint parentUUID = gameObject->LoadNodeData(&node_data);
		createdObjects.push_back(gameObject);

		//check if it's the root game object
		if (strcmp(gameObject->GetName(), "RootNode") == 0) {
			root = gameObject;
		}

		//Get game object's parent
		for (size_t i = 0; i < createdObjects.size(); i++)
		{
			if (createdObjects[i]->UUID == parentUUID)
			{
				createdObjects[i]->AddChild(gameObject);
				gameObject->SetParent(createdObjects[i]);
			}
		}
	}

	model_data.Release();
	RELEASE_ARRAY(buffer);
	App->scene->GetRoot()->AddChild(root);

	return root;
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

void MeshImporter::Load(const char* fileBuffer, ResourceMesh* mesh)
{
	Timer timer;
	timer.Start();

	char* buffer = nullptr;

	FileSystem::Load(fileBuffer, &buffer);

	char* cursor = buffer;

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

	LOG("%s loaded in %.3f s", fileBuffer, timer.ReadSec());

	//mesh->assetsFile = new char[strlen(fileBuffer)];
	//strcpy(mesh->assetsFile.c_str(), fileBuffer);

	//mesh->GenerateBuffers();
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
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		LOG_ERROR("%s, %d %s", texture->assetsFile, ilGetError(), iluErrorString(error));
		return;
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
		LOG("Texture loaded successfully from: %s in %d ms", texture->assetsFile, timer.Read());

		texture->data = ilGetData();
		texture->id = (uint)(imageID);
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	}

	ilBindImage(0);
}

uint TextureImporter::Save(ResourceTexture* texture, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilBindImage(texture->id);

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

void TextureImporter::Load(const char* path, ResourceTexture* texture)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error = IL_NO_ERROR;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	char* buffer;
	uint size = FileSystem::Load(path, &buffer);

	if (ilLoadL(IL_DDS, buffer, size) == IL_FALSE)
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
		LOG("Texture loaded successfully from: %s in %d ms", texture->assetsFile, timer.Read());

		texture->data = ilGetData();
		texture->id = (uint)(imageID);
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
	}

	ilBindImage(0);
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

#pragma endregion

#pragma region MaterialImporter

void MaterialImporter::Import(const aiMaterial* aimaterial, ResourceMaterial* material)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{
		std::string file_path = material->assetsFile;
		file_path = TextureImporter::FindTexture(path.C_Str(), material->assetsFile.c_str());

		if(file_path.size() > 0)
			material->diffuseTextureUID = App->resources->ImportFile(file_path.c_str());

		//LOG("%s imported in %.3f s", texture->path.c_str(), timer.ReadSec());
	}
}

uint64 MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	GnJSONObj base_object;
	base_object.AddInt("Diffuse Texture", material->diffuseTextureUID);

	char* buffer;
	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

void MaterialImporter::Load(const char* path, ResourceMaterial* material)
{
	Timer timer;
	timer.Start();

	char* buffer = nullptr;
	FileSystem::Load(path, &buffer);

	GnJSONObj material_data(buffer);
	material->diffuseTextureUID = material_data.GetInt("Diffuse Texture");

	App->resources->CreateResourceData(material->diffuseTextureUID, ResourceType::RESOURCE_TEXTURE);
	App->resources->LoadResource(material->diffuseTextureUID);

	material_data.Release();
	RELEASE_ARRAY(buffer);
}

#pragma endregion