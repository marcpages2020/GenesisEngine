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
		aiNode* rootNode = scene->mRootNode;

		GnJSONObj save_file;
		GnJSONArray nodes = save_file.AddArray("Nodes");
		GnJSONArray meshes = save_file.AddArray("Meshes");
		GnJSONArray materials = save_file.AddArray("Materials");
		GnJSONArray textures = save_file.AddArray("Textures");

		//Import Meshes
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* aimesh = scene->mMeshes[i];
			ResourceMesh* mesh = (ResourceMesh*)App->resources->CreateResource(model->assetsFile.c_str(), ResourceType::RESOURCE_MESH);
			meshes.AddInt(mesh->GetUID());

			MeshImporter::Import(aimesh, mesh);

			char* buffer;
			uint size = MeshImporter::Save(mesh, &buffer);
			App->resources->SaveResource(buffer, size, mesh);

			App->resources->ReleaseResource(mesh->GetUID());
			RELEASE_ARRAY(buffer);
		}

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* aimaterial = scene->mMaterials[i];
			ResourceMaterial* material = (ResourceMaterial*)App->resources->CreateResource(model->assetsFile.c_str(), ResourceType::RESOURCE_MATERIAL);
			materials.AddInt(material->GetUID());

			MaterialImporter::Import(aimaterial, material, model->assetsFile.c_str());

			//char* buffer;
			//uint size = MaterialImporter::Save(material, &buffer);
			//App->resources->SaveResource(buffer, size, material);

			//App->resources->ReleaseResource(material->GetUID());
			//RELEASE_ARRAY(buffer);
		}

		ImportChildren(scene, rootNode, nullptr, 0, model->assetsFile.c_str(), nodes);

		char* buffer;
		uint size = save_file.Save(&buffer);
		FileSystem::Save(model->libraryFile.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);

		aiReleaseImport(scene);
		save_file.Release();

		LOG("%s loaded in %d ms", model->assetsFile.c_str(), timer.Read());
	}
	else
		LOG_ERROR("Error loading scene %s", model->assetsFile.c_str());
}

void ModelImporter::ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, uint parentNodeUID, const char* path, GnJSONArray& meshes_array)
{
	GnJSONObj node_object;

	//Node Information 
	meshes_array.AddObject(node_object);
	node_object.AddString("Name", node->mName.C_Str());
	uint node_uid = App->resources->GenerateUID();
	node_object.AddInt("Node UID", node_uid);
	node_object.AddInt("ParentNode UID", parentNodeUID);

	LoadTransform(node, node_object);

	if (node->mMeshes != nullptr)
	{
		//Mesh --------------------------------------------------------------

		node_object.AddInt("Mesh Index", *node->mMeshes);

		//Materials ----------------------------------------------------------

//		node_object.AddInt("M")

		/*
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];
		ResourceMaterial* material = (ResourceMaterial*)App->resources->CreateResource(path, ResourceType::RESOURCE_MATERIAL);

		////Import Material
		MaterialImporter::Import(aimaterial, material, path);

		char* materialBuffer;
		size = MaterialImporter::Save(material, &materialBuffer);
		App->resources->SaveResource(materialBuffer, size, material);

		node_object.AddInt("Material UID", material->GetUID());

		//App->resources->ReleaseResource(material->GetUID());
		//RELEASE_ARRAY(materialBuffer);
		*/
	}

	meshes_array.AddObject(node_object);

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ImportChildren(scene, node->mChildren[i], node, node_uid, path, meshes_array);
	}
}

void ModelImporter::LoadTransform(aiNode* node, GnJSONObj& node_object)
{
	aiVector3D position, scaling, eulerRotation;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, position);
	//eulerRotation = rotation.GetEuler() * RADTODEG;

	node_object.AddFloat3("Position", float3(position.x, position.y, position.z));
	node_object.AddQuaternion("Rotation", Quat(rotation.x, rotation.y, rotation.z, rotation.w));
	node_object.AddFloat3("Scale", float3(scaling.x, scaling.y, scaling.z));
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

uint TextureImporter::Save(char** fileBuffer, ResourceTexture* texture)
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

void MaterialImporter::Import(const aiMaterial* aimaterial, ResourceMaterial* material, const char* folder_path)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{
		std::string file_path = folder_path;
		file_path = TextureImporter::FindTexture(path.C_Str(), folder_path);

		if(file_path.size() > 0)
			material->diffuse_texture_uid = App->resources->ImportFile(file_path.c_str());

		//LOG("%s imported in %.3f s", texture->path.c_str(), timer.ReadSec());
	}
}

uint64 MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	//Save Material as JSON
	return 0;
}

void MaterialImporter::Load(const char* fileBuffer, Material* material)
{
	Timer timer;
	timer.Start();
	
}

#pragma endregion