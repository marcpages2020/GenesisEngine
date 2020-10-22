#include "FileSystem.h"
#include "Mesh.h"
#include "Application.h"

#include "SDL/include/SDL.h"
#include <fstream>
#include <iostream>
#include <Shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

#include "PhysFS/include/physfs.h"
#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"

#include "Devil/include/IL/il.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"


#pragma comment (lib, "PhysFS/libx86/physfs.lib")
#pragma comment (lib, "Assimp/Assimp/libx86/assimp.lib")

#pragma comment (lib, "Devil/libx86/DevIL.lib")	
#pragma comment (lib, "Devil/libx86/ILU.lib")	
#pragma comment (lib, "Devil/libx86/ILUT.lib")	

void FileSystem::Init()
{
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	AddPath("."); //Adding ProjectFolder (working directory)
	AddPath("Assets");

	if (PHYSFS_setWriteDir(".") == 0)
		LOG_ERROR("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	CreateLibraryDirectories();

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void FileSystem::CleanUp()
{
	PHYSFS_deinit();
	aiDetachAllLogStreams();
}

void FileSystem::CreateLibraryDirectories()
{
	//CreateDir(LIBRARY_PATH);
	//CreateDir(FOLDERS_PATH);
	//CreateDir(MESHES_PATH);
	//CreateDir(TEXTURES_PATH);
	//CreateDir(MATERIALS_PATH);
	CreateDir("Assets/Models/model");
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

//TODO
/*
PathNode FileSystem::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext) 
{
	PathNode root;
	if (Exists(directory))
	{
		root.path = directory;
		Engine->fileSystem->SplitFilePath(directory, nullptr, &root.localPath);
		if (root.localPath == "")
			root.localPath = directory;

		std::vector<std::string> file_list, dir_list;
		DiscoverFiles(directory, file_list, dir_list);

		//Adding all child directories
		for (uint i = 0; i < dir_list.size(); i++)
		{
			std::string str = directory;
			str.append("/").append(dir_list[i]);
			root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
		}
		//Adding all child files
		for (uint i = 0; i < file_list.size(); i++)
		{
			//Filtering extensions
			bool filter = true, discard = false;
			if (filter_ext != nullptr)
			{
				filter = HasExtension(file_list[i].c_str(), *filter_ext);
			}
			if (ignore_ext != nullptr)
			{
				discard = HasExtension(file_list[i].c_str(), *ignore_ext);
			}
			if (filter == true && discard == false)
			{
				std::string str = directory;
				str.append("/").append(file_list[i]);
				root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
			}
		}
		root.isFile = HasExtension(root.path.c_str());
		root.isLeaf = root.children.empty() == true;
	}
	return root;
}
*/

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
	std::string ret;
	GetRealDir(originalPath, ret);

	return ret;
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
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
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
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

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
			LOG("[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
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
			LOG("[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG("[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
}

//TODO
/*
bool FileSystem::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		//If it is a directory, we need to recursively remove all the files inside
		if (IsDirectory(file))
		{
			std::vector<std::string> containedFiles, containedDirs;
			PathNode rootDirectory = GetAllFiles(file);

			for (uint i = 0; i < rootDirectory.children.size(); ++i)
				Remove(rootDirectory.children[i].path.c_str());
		}

		if (PHYSFS_delete(file) != 0)
		{
			LOG("File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG("File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}

	return ret;
}
*/

//TODO
/*
uint64 FileSystem::GetLastModTime(const char* filename)
{
	return PHYSFS_getLastModTime(filename);
}
*/

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

void FileSystem::LoadFile(const char* file_path)
{
	std::string extension = PathFindExtensionA(file_path);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

	if (extension == ".fbx") 
	{
		App->renderer3D->AddMeshCollection(LoadFBX(file_path));
	}
	else if (extension == ".png")
	{
		LoadTexture(file_path);
	}
}

MeshCollection* FileSystem::LoadFBX(const char* path)
{
	MeshCollection* collection = nullptr;
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		collection = new MeshCollection();

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* currentMesh = new Mesh();
			aiMesh* currentAiMesh = scene->mMeshes[i];

			//vertex copying
			currentMesh->vertices_amount = currentAiMesh->mNumVertices;
			currentMesh->vertices = new float[currentMesh->vertices_amount * 3]();
			memcpy(currentMesh->vertices, currentAiMesh->mVertices, sizeof(float) * currentMesh->vertices_amount * 3);
			LOG("%s loaded with %d vertices", currentAiMesh->mName.C_Str(), currentMesh->vertices_amount);

			//face copying
			if (currentAiMesh->HasFaces()) 
			{
				currentMesh->indices_amount = currentAiMesh->mNumFaces * 3;
				currentMesh->indices = new uint[currentMesh->indices_amount]();
				LOG("%s loaded with %d indices", currentAiMesh->mName.C_Str(), currentMesh->indices_amount);

				for (size_t f = 0; f < currentAiMesh->mNumFaces; f++)
				{
					if (currentAiMesh->mFaces[f].mNumIndices != 3) 
					{
						LOG_WARNING("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&currentMesh->indices[f * 3], currentAiMesh->mFaces[f].mIndices, 3 * sizeof(uint));
					}
				}
			}

			currentMesh->texcoords = new float[currentMesh->vertices_amount * 2]();
			currentMesh->colors = new float[currentMesh->indices_amount * 4]();
			currentMesh->normals = new float[currentAiMesh->mNumVertices * 3]();

			int t = 0;

			for (size_t v = 0, n = 0, tx = 0, c = 0; v < currentAiMesh->mNumVertices; v++, n += 3, c += 4, tx+= 2)
			{
				//normal copying
				if (currentAiMesh->HasNormals())
				{
					//normal copying
					currentMesh->normals[n] = currentAiMesh->mNormals[v].x;
					currentMesh->normals[n + 1] = currentAiMesh->mNormals[v].y;
					currentMesh->normals[n + 2] = currentAiMesh->mNormals[v].z;
				}
				//color copying
				if (currentAiMesh->HasVertexColors(v)) 
				{
					currentMesh->colors[c] = currentAiMesh->mColors[v]->r;
					currentMesh->colors[c + 1] = currentAiMesh->mColors[v]->g;
					currentMesh->colors[c + 2] = currentAiMesh->mColors[v]->b;
					currentMesh->colors[c + 3] = currentAiMesh->mColors[v]->a;
				}
				else 
				{
					currentMesh->colors[c] = 0.0f;
					currentMesh->colors[c + 1] = 0.0f;
					currentMesh->colors[c + 2] = 0.0f;
					currentMesh->colors[c + 3] = 0.0f;
				}

				//texcoords copying
				if (currentAiMesh->mTextureCoords[0]) 
				{
					currentMesh->texcoords[tx] = currentAiMesh->mTextureCoords[0][v].x;
					currentMesh->texcoords[tx + 1] = currentAiMesh->mTextureCoords[0][v].y;
					//LOG("TexCoords: %.2f , %.2f", currentMesh->texcoords[tx], currentMesh->texcoords[tx + 1]);
				}
				else
				{
					currentMesh->texcoords[tx] = 0.0f;
					currentMesh->texcoords[tx + 1] = 0.0f;
				}
				t = tx;
			}
			//LOG("Texcoords loaded: %d", t);
			currentMesh->GenerateBuffers();
			collection->meshes.push_back(currentMesh);
		}

		aiReleaseImport(scene);
	}
	else
		LOG_ERROR("Error loading scene %s", path);

	return collection;
}

Texture FileSystem::LoadTexture(const char* path)
{
	uint imageID = 0;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	Texture texture;

	if (imageID == 0)
		LOG_ERROR("Could not create a texture buffer to load: %s, %d", path, ilGetError());
	
	if (ilLoadImage(path) == IL_FALSE)
		LOG_ERROR("Error trying to load the texture from %s", path);

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	
	texture.id = imageID;
	texture.data = ilGetData();
	texture.width = ilGetInteger(IL_IMAGE_WIDTH);
	texture.height = ilGetInteger(IL_IMAGE_HEIGHT);

	//ilBindImage(0);
	//ilDeleteImages(1, &imageID);

	ILenum error;
	error = ilGetError();

	if (error != IL_NO_ERROR) {
		LOG_ERROR("%d: %s", error, iluErrorString(error));
	}
	else {
		LOG("Texture: %s loaded successfully", path);}

	return texture;
}

void FileSystem::UnloadTexture(uint imageID)
{
	ilBindImage(0);
	ilDeleteImages(1, &imageID);
}

