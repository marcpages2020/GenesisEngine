#include "Importers.h"
#include "Timer.h"
#include "FileSystem.h"

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

GameObject* ModelImporter::Import(const char* full_path)
{
	Timer timer;
	timer.Start();

	GameObject* root = nullptr;

	std::string folder, file;
	FileSystem::SplitFilePath(full_path, &folder, &file);
	std::string path = FileSystem::GetPathRelativeToAssets(full_path);

	const aiScene* scene = nullptr;

	if (FileSystem::Exists(path.c_str()))
	{
		char* buffer = nullptr;
		uint size = FileSystem::Load(path.c_str(), &buffer);

		scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, NULL);

		RELEASE_ARRAY(buffer);
	}
	else
	{
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	}

	if (scene != nullptr && scene->HasMeshes())
	{
		aiNode* rootNode = scene->mRootNode;

		GnJSONObj save_file;
		GnJSONArray meshes = save_file.AddArray("Meshes");

		root = ImportChildren(scene, rootNode, nullptr, nullptr, folder.c_str(), meshes);
		root->SetName(file.c_str());

		char* buffer = nullptr;
		uint size = save_file.Save(&buffer);
		path = "Library/Models/";
		path += file.c_str();
		path += ".model";
		FileSystem::Save(path.c_str(), buffer, size);

		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);
		save_file.Release();

		LOG("%s loaded in %.3f s", full_path, timer.ReadSec());
	}
	else
		LOG_ERROR("Error loading scene %s", full_path);

	//root->UpdateChildrenTransforms();

	return root;
}

GameObject* ModelImporter::ImportChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, GameObject* parentGameObject, const char* path, GnJSONArray& meshes_array)
{
	GameObject* gameObject = new GameObject();
	gameObject->SetName(node->mName.C_Str());

	GnJSONObj save_object;

	if (parentGameObject != nullptr)
	{
		parentGameObject->AddChild(gameObject);
		gameObject->SetParent(parentGameObject);
	}

	if (node->mMeshes != nullptr)
	{
		//Transform------------------------------------------------------------

		LoadTransform(node, gameObject->GetTransform());
		gameObject->GetTransform()->UpdateGlobalTransform(parentGameObject->GetTransform()->GetGlobalTransform());

		//Mesh --------------------------------------------------------------

		char* meshBuffer = nullptr;
		GnMesh* mesh = new GnMesh();
		aiMesh* aimesh = scene->mMeshes[*node->mMeshes];

		MeshImporter::Import(aimesh, mesh);
		uint size = MeshImporter::Save(mesh, &meshBuffer);

		char file_path[128];
		sprintf_s(file_path, 128, "Library/Meshes/%s.mesh", node->mName.C_Str());
		FileSystem::Save(file_path, meshBuffer, size);
		save_object.AddString("Mesh: ", file_path);

		delete mesh;
		mesh = nullptr;

		mesh = new GnMesh();
		MeshImporter::Load(file_path, mesh);
		gameObject->AddComponent(mesh);

		RELEASE_ARRAY(meshBuffer);

		//Materials ----------------------------------------------------------

		char* texBuffer = nullptr;

		Material* material = new Material();
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		//Import Material
		MaterialImporter::Import(aimaterial, material, path);
		size = MaterialImporter::Save(material, &texBuffer);

		//Set own format path
		std::string textures_path = "Library/Textures/";
		std::string file = FileSystem::GetFile(material->GetDiffuseTexture()->name.c_str());
		textures_path += file;
		textures_path += ".dds";

		//Save into Library
		FileSystem::Save(textures_path.c_str(), texBuffer, size);
		save_object.AddString("Diffuse Texture: ", textures_path.c_str());

		delete material;
		material = nullptr;
		material = new Material();

		MaterialImporter::Load(textures_path.c_str(), material);

		material->SetMesh(mesh);
		gameObject->AddComponent(material);

		RELEASE_ARRAY(texBuffer);

		meshes_array.AddObject(save_object);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ImportChildren(scene, node->mChildren[i], node, gameObject, path, meshes_array);
	}

	return gameObject;
}

void ModelImporter::LoadTransform(aiNode* node, Transform* transform)
{
	aiVector3D position, scaling, eulerRotation;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, position);
	eulerRotation = rotation.GetEuler() * RADTODEG;

	transform->SetPosition(position.x, position.y, position.z);
	transform->SetRotation(eulerRotation.x, eulerRotation.y, eulerRotation.z);

	if (FileSystem::normalize_scales) {
		if (scaling.x == 100 && scaling.y == 100 && scaling.z == 100)
		{
			scaling.x = scaling.y = scaling.z = 1.0f;
		}
	}

	transform->SetScale(scaling.x, scaling.y, scaling.z);
	transform->UpdateLocalTransform();
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

void MeshImporter::Import(const aiMesh* aimesh, GnMesh* mesh)
{
	Timer timer;
	timer.Start();

	mesh->name = aimesh->mName.C_Str();

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

	LOG("Mesh imported from Assimp in %.3f s", timer.ReadSec());
}

uint64 MeshImporter::Save(GnMesh* mesh, char** fileBuffer)
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
	//cursor += bytes;

	*fileBuffer = buffer;

	//RELEASE_ARRAY(buffer);

	return size;
}

void MeshImporter::Load(const char* fileBuffer, GnMesh* mesh)
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

	mesh->path = new char[strlen(fileBuffer)];
	strcpy(mesh->path, fileBuffer);

	mesh->GenerateBuffers();
}

#pragma endregion 

#pragma region TextureImporter



void TextureImporter::Import(const char* path)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	GnTexture* texture = new GnTexture();

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		error = ilGetError();
		LOG_ERROR("Could not create a texture buffer to load: %s, %d %s", path, ilGetError(), iluErrorString(error));
	}

	ILenum file_format = GetFileFormat(path);

	if (FileSystem::Exists(path))
	{
		char* buffer;
		uint size = FileSystem::Load(path, &buffer);

		if (ilLoadL(file_format, buffer, size) == IL_FALSE)
		{
			LOG_WARNING("Warning: Trying to load the texture %s into buffer, %d: %s", path, ilGetError(), iluErrorString(ilGetError()));
			buffer = nullptr;

			//Reset Image
			ilBindImage(0);
			ilDeleteImages(1, &imageID);
			//return nullptr;
		}

		if (buffer != NULL)
			RELEASE_ARRAY(buffer);
	}
	else
	{
		if (ilLoadImage(path) == IL_FALSE)
		{
			LOG_ERROR("Error trying to load the texture directly from %s", path);
		}
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
		LOG("Texture loaded successfully from: %s in %.3f s", path, timer.ReadSec());

		texture->id = (uint)(imageID);
		texture->name = FileSystem::GetFile(path);
		texture->data = ilGetData();
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
		texture->path = path;
	}

	ilBindImage(0);

	//return texture;
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

void MaterialImporter::Init()
{
	ilInit();
	iluInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL initted correctly");
}

void MaterialImporter::Import(const aiMaterial* aimaterial, Material* material, const char* folder_path)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{
		std::string file_path = folder_path;
		file_path += path.C_Str();
		GnTexture* texture = LoadTexture(file_path.c_str());

		material->SetTexture(texture);

		LOG("%s imported in %.3f s", texture->path.c_str(), timer.ReadSec());
	}
}

uint64 MaterialImporter::Save(Material* ourMaterial, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilBindImage(ourMaterial->GetDiffuseTexture()->id);

	ilSetInteger(IL_DXTC_DATA_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0)
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;

		//RELEASE_ARRAY(data);
	}
	else 
	{
		ILenum error;
		error = ilGetError();

		if (error != IL_NO_ERROR)
		{
			LOG_ERROR("Error when saving %s - %d: %s", ourMaterial->GetDiffuseTexture()->name, error, iluErrorString(error));
		}
	}

	ilBindImage(0);

	return size;
}

void MaterialImporter::Load(const char* fileBuffer, Material* material)
{
	Timer timer;
	timer.Start();
	ILuint imageID = 0;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	char* buffer = nullptr;
	uint size = FileSystem::Load(fileBuffer, &buffer);

	if (ilLoadL(IL_DDS, buffer, size) == IL_FALSE)
	{
		LOG_WARNING("Error when trying to load the texture %s into buffer, %d: %s", fileBuffer, ilGetError(), iluErrorString(ilGetError()));
		buffer = nullptr;

		//Reset Image
		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		ilGenImages(1, &imageID);
		ilBindImage(imageID);
	}

	if (buffer != NULL)
		RELEASE_ARRAY(buffer);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ILenum error = IL_NO_ERROR;
	error = ilGetError();

	if (error != IL_NO_ERROR)
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		LOG_ERROR("Error when loading %s - %d: %s", fileBuffer, error, iluErrorString(error));
	}
	else
	{
		GnTexture* texture = new GnTexture();

		texture->id = (uint)(imageID);
		//texture->name = FileSystem::GetFile(f) + format;
		texture->data = ilGetData();
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
		texture->path = fileBuffer;

		material->SetTexture(texture);

		LOG("Texture loaded successfully from: %s in %.3f s", fileBuffer, timer.ReadSec());
	}

	ilBindImage(0);
}

GnTexture* MaterialImporter::LoadTexture(const char* full_path)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error;

	std::string normalized_path = FileSystem::NormalizePath(full_path);
	std::string relative_path = FileSystem::GetPathRelativeToAssets(normalized_path.c_str());

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	GnTexture* texture = new GnTexture();

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		error = ilGetError();
		LOG_ERROR("Could not create a texture buffer to load: %s, %d %s", full_path, ilGetError(), iluErrorString(error));
	}

	ILenum file_format = IL_TYPE_UNKNOWN;
	std::string format = FileSystem::GetFileFormat(full_path);

	if (format == ".png")
		file_format = IL_PNG;
	else if (format == ".jpg")
		file_format = IL_JPG;
	else if (format == ".bmp")
		file_format = IL_BMP;

	if (FileSystem::Exists(relative_path.c_str()))
	{
		char* buffer;
		uint size = FileSystem::Load(relative_path.c_str(), &buffer);

		if (ilLoadL(file_format, buffer, size) == IL_FALSE)
		{
			LOG_WARNING("Warning: Trying to load the texture %s into buffer, %d: %s", full_path, ilGetError(), iluErrorString(ilGetError()));
			buffer = nullptr;

			//Reset Image
			ilBindImage(0);
			ilDeleteImages(1, &imageID);
			return nullptr;
		}

		if (buffer != NULL)
			RELEASE_ARRAY(buffer);
	}
	else
	{
		if (ilLoadImage(normalized_path.c_str()) == IL_FALSE)
		{
			LOG_ERROR("Error trying to load the texture directly from %s", full_path);
		}
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
		LOG("Texture loaded successfully from: %s in %.3f s", full_path, timer.ReadSec());

		texture->id = (uint)(imageID);
		texture->name = FileSystem::GetFile(full_path) + format;
		texture->data = ilGetData();
		texture->width = ilGetInteger(IL_IMAGE_WIDTH);
		texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
		texture->path = normalized_path.c_str();
	}

	ilBindImage(0);

	return texture;
}

#pragma endregion