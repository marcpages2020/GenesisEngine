#pragma once

class MeshCollection;
#include <vector>

namespace FileSystem
{
	void Init();
	MeshCollection* LoadFBX(const char* path);
	void CleanUp();
}


