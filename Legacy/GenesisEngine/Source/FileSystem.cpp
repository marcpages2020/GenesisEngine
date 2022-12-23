#include "FileSystem.h"
#include "Application.h"

#include "SDL/include/SDL.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <Shlwapi.h>

#include "PhysFS/include/physfs.h"

#pragma comment (lib, "PhysFS/libx86/physfs.lib")
#pragma comment (lib,"shlwapi.lib")

#pragma region FileSystem

void FileSystem::Init()
{
	char* base_path = SDL_GetBasePath();

	if (PHYSFS_init(nullptr) != 0)
		LOG("PhysFS initted correctly");
	
	SDL_free(base_path);

	AddPath("."); //Adding ProjectFolder (working directory)
	AddPath("Assets");

	if (PHYSFS_setWriteDir(".") == 0)
		LOG_ERROR("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	CreateLibraryDirectories();

	std::string path = PHYSFS_getWriteDir();
}

void FileSystem::CleanUp()
{
	PHYSFS_deinit();
}

void FileSystem::GetPhysFSVersion(std::string& version_str)
{
	PHYSFS_Version version;
	PHYSFS_getLinkedVersion(&version);
	version_str = std::to_string(version.major) + "." + std::to_string(version.minor) + "." + std::to_string(version.patch);
}

void FileSystem::CreateLibraryDirectories()
{
	CreateDir("Assets/Config/");
	CreateDir("Assets/Textures/");
	CreateDir("Assets/Models/");
	CreateDir("Assets/Scenes/");
	CreateDir("Assets/Shaders/");

	CreateDir("Library/Config/");
	CreateDir("Library/Models/");
	CreateDir("Library/Meshes/");
	CreateDir("Library/Materials/");
	CreateDir("Library/Textures/");
	CreateDir("Library/Scenes/");
	CreateDir("Library/Shaders/");

}

// Add a new zip file_path or folder
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

// Check if a file_path exists
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

// Check if a file_path is a directory
bool FileSystem::IsDirectory(const char* file) 
{
	return PHYSFS_isDirectory(file) != 0;
}

const char* FileSystem::GetWriteDir() 
{
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

void FileSystem::DiscoverFilesRecursive(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list)
{
	std::vector<std::string> files;
	std::vector<std::string> directories;

	DiscoverFiles(directory, files, directories);
	
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string file = directory;
		file.append("/" + files[i]);
		file_list.push_back(file);
	}

	for (size_t i = 0; i < directories.size(); i++) 
	{
		std::string dir = directory;
		dir.append("/" + directories[i]);
		dir_list.push_back(dir);
		DiscoverFilesRecursive(dir.c_str(), file_list, dir_list);
	}

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

	//The file_path is inside a directory
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

void FileSystem::SplitFilePath(const char* full_path, std::vector<std::string>* splits)
{
	std::string tmp;
	std::string substring;

	tmp = full_path;
	while (tmp.find("/") != std::string::npos)
	{
		size_t found = tmp.find("/");
		substring = tmp.substr(0, found);
		tmp.erase(tmp.begin(), tmp.begin() + found +1);
		splits->push_back(substring);
	}

	splits->push_back(tmp);
}

unsigned int FileSystem::Load(const char* path, const char* file, char** buffer) 
{
	std::string full_path(path);
	full_path += file;
	return Load(full_path.c_str(), buffer);
}

// Read a whole file_path and put it in a new buffer
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
				//Adding end of file_path at the end of the buffer. Loading a shader file_path does not add this for some reason
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
		LOG_ERROR("File %s could not be duplicated into %s, %s", srcFile, dstFile , PHYSFS_getLastError());
		return false;
	}
}

void FileSystem::Rename(const char* old_name, const char* new_name)
{
	char* fileBuffer;
	uint size = FileSystem::Load(old_name, &fileBuffer);
	
	if(size > 0)
	{
		Save(new_name, fileBuffer, size);
		Delete(old_name);

		RELEASE_ARRAY(fileBuffer);
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

bool FileSystem::Delete(const char* file)
{
	if (PHYSFS_delete(file) != 0)
		return true;
	else {
		LOG_ERROR("File System error while deleting file %s: %s", file, PHYSFS_getLastError());
		return false;
	}
}

uint64 FileSystem::GetLastModTime(const char* filename)
{
	return PHYSFS_getLastModTime(filename);
}

std::string FileSystem::GetUniqueName(const char* path, const char* name)
{
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

std::string FileSystem::GetFileExtension(const char* path)
{
	std::string format = PathFindExtensionA(path);
	std::transform(format.begin(), format.end(), format.begin(), [](unsigned char c) { return std::tolower(c); });
	return format;
}

std::string FileSystem::GetFile(const char* path)
{
	std::string file;
	std::string file_path;
	std::string extension;
	SplitFilePath(path, &file_path, &file, &extension);
	return file + "." + extension;
}

std::string FileSystem::GetFileName(const char* path)
{
	std::string file;
	std::string file_path;
	SplitFilePath(path, &file_path, &file);
	return file;
}

std::string FileSystem::GetFolder(const char* path)
{
	std::string folder;
	SplitFilePath(path, &folder);
	return folder;
}

std::string FileSystem::ToLower(const char* path)
{
	std::string string = path;
	std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::tolower(c); });
	return string;
}



#pragma endregion 

