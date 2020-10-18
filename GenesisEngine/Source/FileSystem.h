#pragma once

class MeshCollection;
#include <vector>

namespace FileSystem
{
	void Init();
	void CleanUp();

	MeshCollection* LoadFBX(const char* path);
	void Save();
}


