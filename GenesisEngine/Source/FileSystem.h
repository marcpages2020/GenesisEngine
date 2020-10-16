#pragma once

class Mesh;
#include <vector>

namespace FileSystem
{
	void Init();
	std::vector<Mesh*> LoadFBX(const char* path);
	void CleanUp();
}


