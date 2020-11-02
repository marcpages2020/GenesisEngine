#include "FileSystem.h"
#include "Application.h"

#include "Mesh.h"
#include "GameObject.h"
#include "Material.h"
#include "Transform.h"

#include "SDL/include/SDL.h"
#include <fstream>
#include <iostream>
#include <Shlwapi.h>

#include "Timer.h"
#include "parson/parson.h"

#include "PhysFS/include/physfs.h"
#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"

#include "Devil/include/IL/il.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#pragma comment (lib, "PhysFS/libx86/physfs.lib")
#pragma comment (lib,"shlwapi.lib")
#pragma comment (lib, "Assimp/Assimp/libx86/assimp.lib")

#pragma comment (lib, "Devil/libx86/DevIL.lib")	
#pragma comment (lib, "Devil/libx86/ILU.lib")	
#pragma comment (lib, "Devil/libx86/ILUT.lib")	

#pragma region FileSystem

void FileSystem::Init()
{
	ilInit();
	iluInit();
	if (ilutRenderer(ILUT_OPENGL)) 
		LOG("DevIL initted correctly");

	char* base_path = SDL_GetBasePath();

	if (PHYSFS_init(nullptr) != 0)
		LOG("PhysFS initted correctly");
	
	SDL_free(base_path);

	AddPath("."); //Adding ProjectFolder (working directory)
	AddPath("Assets");

	if (PHYSFS_setWriteDir(".") == 0)
		LOG_ERROR("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	CreateLibraryDirectories();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	std::string path = PHYSFS_getWriteDir();

	normalize_scales = true;
}

void FileSystem::CleanUp()
{
	PHYSFS_deinit();
	aiDetachAllLogStreams();
}

void FileSystem::GetPhysFSVersion(std::string& version_str)
{
	PHYSFS_Version version;
	PHYSFS_getLinkedVersion(&version);
	version_str = std::to_string(version.major) + "." + std::to_string(version.minor) + "." + std::to_string(version.patch);
}

void FileSystem::CreateLibraryDirectories()
{
	//CreateDir(LIBRARY_PATH);
	//CreateDir(FOLDERS_PATH);
	//CreateDir(MESHES_PATH);
	//CreateDir(TEXTURES_PATH);
	//CreateDir(MATERIALS_PATH);
	CreateDir("Assets/Config/");
	CreateDir("Assets/Textures/");
	CreateDir("Assets/Models/");

	CreateDir("Library/Config/");
	CreateDir("Library/Textures/");
	CreateDir("Library/Meshes/");
	//CreateDir("Materials/");
	//CreateDir(ANIMATIONS_PATH);
	//CreateDir(PARTICLES_PATH);
	//CreateDir(SHADERS_PATH);
	//CreateDir(SCENES_PATH);
}

// Add a new zip file or folder
bool FileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG_ERROR("File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

// Check if a file exists
bool FileSystem::Exists(const char* file) 
{
	return PHYSFS_exists(file) != 0;
}

bool FileSystem::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return false;
}

// Check if a file is a directory
bool FileSystem::IsDirectory(const char* file) 
{
	return PHYSFS_isDirectory(file) != 0;
}

const char* FileSystem::GetWriteDir() 
{
	//TODO: erase first annoying dot (".")
	return PHYSFS_getWriteDir();
}

void FileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) 
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

void FileSystem::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) 
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension)
			file_list.push_back(files[i]);
	}
}

void FileSystem::GetRealDir(const char* path, std::string& output) 
{
	output = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	output.append(*PHYSFS_getSearchPath()).append("/");
	output.append(PHYSFS_getRealDir(path)).append("/").append(path);
}

std::string FileSystem::GetPathRelativeToAssets(const char* originalPath) 
{
	std::string file_path = originalPath;
	
	std::size_t pos = file_path.find("Assets");

	if (pos > file_path.size())
	{
		file_path.clear();
	}
	else
	{
		file_path = file_path.substr(pos);
	}

	return file_path;
}

bool FileSystem::HasExtension(const char* path) 
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";
}

bool FileSystem::HasExtension(const char* path, std::string extension) 
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;
}

bool FileSystem::HasExtension(const char* path, std::vector<std::string> extensions) 
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "")
		return true;
	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
}

std::string FileSystem::ProcessPath(const char* path)
{
	std::string final_path = path;
	
	final_path = NormalizePath(final_path.c_str());
	std::string tmp_path = GetPathRelativeToAssets(final_path.c_str());

	//The file is inside a directory
	if (tmp_path.size() > 0)
	{
		return tmp_path.c_str();
	}

	return final_path.c_str();
}

std::string FileSystem::NormalizePath(const char* full_path)
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}

void FileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) 
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

unsigned int FileSystem::Load(const char* path, const char* file, char** buffer) 
{
	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint FileSystem::Load(const char* file, char** buffer) 
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG_ERROR("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG_ERROR("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG_ERROR("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

bool FileSystem::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath)
{
	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}

bool FileSystem::DuplicateFile(const char* srcFile, const char* dstFile)
{
	//TODO: Compare performance to calling Load(srcFile) and then Save(dstFile)
	std::ifstream src;
	src.open(srcFile, std::ios::binary);
	bool srcOpen = src.is_open();
	std::ofstream  dst(dstFile, std::ios::binary);
	bool dstOpen = dst.is_open();

	dst << src.rdbuf();

	src.close();
	dst.close();

	if (srcOpen && dstOpen)
	{
		LOG("[success] File Duplicated Correctly");
		return true;
	}
	else
	{
		LOG("[error] File could not be duplicated");
		return false;
	}
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
uint FileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) 
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG_ERROR("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			if (append == true)
			{
				LOG("Added %u data to [%s%s]", size, GetWriteDir(), file);
			}
			else if (overwrite == true)
			{
				LOG("File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size);
			}
			else
			{
				LOG("New file created [%s%s] of %u bytes", GetWriteDir(), file, size);
			}

			ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG_ERROR("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG_ERROR("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

std::string FileSystem::GetUniqueName(const char* path, const char* name) 
{
	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> files, dirs;
	DiscoverFiles(path, files, dirs);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i)
	{
		unique = true;

		//Build the compare name (name_i)
		if (i > 0)
		{
			finalName = std::string(name).append("_");
			if (i < 10)
				finalName.append("0");
			finalName.append(std::to_string(i));
		}

		//Iterate through all the files to find a matching name
		for (uint f = 0; f < files.size(); ++f)
		{
			if (finalName == files[f])
			{
				unique = false;
				break;
			}
		}
	}
	return finalName;
}

void FileSystem::LoadFile(const char* file_path, bool drag_and_drop)
{
	std::string extension = GetFileFormat(file_path);
	std::string processed_path = ProcessPath(file_path);

	if (extension == ".fbx")
	{
		GameObject* imported_fbx = MeshImporter::ImportFBX(processed_path.c_str());

		if (imported_fbx != nullptr)
			App->scene->AddGameObject(imported_fbx);
	}
	else if (extension == ".png" || extension ==".jpg" || extension == ".bmp" || extension == ".tga")
	{
		if (drag_and_drop)
		{
			//App->scene->SetDroppedTexture(TextureImporter::LoadTexture(processed_path.c_str()));
		}
	}
}

std::string FileSystem::GetFileFormat(const char* path)
{
	std::string format = PathFindExtensionA(path);
	std::transform(format.begin(), format.end(), format.begin(), [](unsigned char c) { return std::tolower(c); });
	return format;
}

std::string FileSystem::GetFile(const char* path)
{
	std::string file;
	std::string file_path;
	SplitFilePath(path, &file_path, &file);
	return file;
}

#pragma endregion 

#pragma region MeshImporter

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

	std::string path = "Library/Meshes/";
	path += mesh->name;
	FileSystem::Save(path.c_str(), buffer, size);

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

	mesh->GenerateBuffers();

	//RELEASE_ARRAY(buffer);

	LOG("%s loaded in %.3f s", mesh->name, timer.ReadSec());
}

GameObject* MeshImporter::ImportFBX(const char* full_path)
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

		size = FileSystem::Load(path.c_str(), &buffer);
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

		root = PreorderChildren(scene, rootNode, nullptr, nullptr, folder.c_str());
		root->SetName(file.c_str());

		aiReleaseImport(scene);
		LOG("%s loaded in %.3f s", full_path, timer.ReadSec());
	}
	else
		LOG_ERROR("Error loading scene %s", full_path);

	//root->UpdateChildrenTransforms();

	return root;
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
	LOG("%s loaded with %d vertices", aimesh->mName.C_Str(), mesh->vertices_amount);

	//indices copying
	if (aimesh->HasFaces())
	{
		mesh->indices_amount = aimesh->mNumFaces * 3;
		mesh->indices = new uint[mesh->indices_amount]();
		LOG("%s loaded with %d indices", aimesh->mName.C_Str(), mesh->indices_amount);

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

	mesh->GenerateBuffers();
}

GameObject* MeshImporter::PreorderChildren(const aiScene* scene, aiNode* node, aiNode* parentNode, GameObject* parentGameObject, const char* path)
{
	GameObject* gameObject = new GameObject();

	if (parentGameObject != nullptr)
	{
		parentGameObject->AddChild(gameObject);
		gameObject->SetParent(parentGameObject);
	}

	if (node->mMeshes != nullptr)
	{
		gameObject->SetName(node->mName.C_Str());

		//Mesh --------------------------------------------------------------

		char* buffer;
		GnMesh* mesh = new GnMesh();
		aiMesh* aimesh = scene->mMeshes[*node->mMeshes];

		Import(aimesh, mesh);
		Save(mesh, &buffer);

		std::string file = "Library/Meshes/";
		file += mesh->name;

		delete mesh;
		mesh = nullptr;

		mesh = new GnMesh();

		Load(file.c_str(), mesh);

		gameObject->AddComponent(mesh);
		//RELEASE_ARRAY(buffer);

		//Materials ----------------------------------------------------------

		Material* material = new Material();
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		MaterialImporter::Import(aimaterial, material, path);
		//MaterialImporter::Save(material, &buffer);
		material->SetMesh(mesh);
		gameObject->AddComponent(material);
		

		//RELEASE_ARRAY(buffer);

		//Transform------------------------------------------------------------
		LoadTransform(node, gameObject->GetTransform());
	}


	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		PreorderChildren(scene, node->mChildren[i], node, gameObject, path);
	}

	return gameObject;
}

void MeshImporter::LoadTransform(aiNode* node, Transform* transform)
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

#pragma region TextureImporter

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

#pragma endregion

#pragma region JSONParser

JSON_Array* JSONParser::LoadConfig(char* buffer, JSON_Value* root)
{
	root = json_parse_string(buffer);
	JSON_Object* config_object = json_value_get_object(root);
	JSON_Array* modules = json_object_get_array(config_object, "modules");

	if (root != NULL)
	{
		LOG("Config file loaded successfully");
	}
	else
	{
		LOG_ERROR("Error trying to load config file");
	}

	return modules;
}

JSON_Object* JSONParser::GetJSONObjectByName(const char* name, JSON_Array* modules_array)
{
	int modules = json_array_get_count(modules_array);

	for (size_t i = 0; i < modules; i++)
	{
		JSON_Object* object = json_array_get_object(modules_array, i);
		if (strcmp(name, json_object_get_string(object, "name")) == 0)
			return object;
	}

	LOG_ERROR("JSON object %s could not be found", name);
	return NULL;
}

#pragma endregion

void MaterialImporter::Import(const aiMaterial* aimaterial, Material* material, const char* folder_path)
{
	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

	if (path.length > 0)
	{ 
		std::string file_path = folder_path;
		file_path += path.C_Str();

		GnTexture* texture = LoadTexture(file_path.c_str());

		material->SetTexture(texture);
	}
}

uint64 MaterialImporter::Save(const Material* ourMaterial, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilSetInteger(IL_DXTC_DATA_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0) 
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
		
		RELEASE_ARRAY(data);
	}

	return size;
}

GnTexture* MaterialImporter::LoadTexture(const char* full_path)
{
	Timer timer;
	timer.Start();
	ILuint imageID = 0;

	std::string normalized_path = FileSystem::NormalizePath(full_path);
	std::string relative_path = FileSystem::GetPathRelativeToAssets(normalized_path.c_str());

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	GnTexture* texture = new GnTexture();

	if (imageID == 0) LOG_ERROR("Could not create a texture buffer to load: %s, %d", full_path, ilGetError());

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
			ilGenImages(1, &imageID);
			ilBindImage(imageID);
		}

		if (buffer != NULL)
			RELEASE_ARRAY(buffer);
	}
	else
	{
		if (ilLoadImage(normalized_path.c_str()) == IL_FALSE)
		{
			LOG_ERROR("Error trying to load the texture directly from %s", full_path);
			//texture = nullptr;
		}
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ILenum error;
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