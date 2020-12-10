#pragma once
#include "EditorWindow.h"

#include <string>

class ResourceTexture;

struct AssetsIcons
{
	ResourceTexture* folder = nullptr;
	ResourceTexture* model = nullptr;
};

class WindowAssets : public EditorWindow {
public:
	WindowAssets();
	~WindowAssets();

	bool Init() override;
	void Draw() override;
	const char* GetFileAt(int i);

private:
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);
	void DrawCurrentFolder();
	bool DrawIcon(const char* path, int id, bool isFolder = false);

private:
	std::string current_folder;
	AssetsIcons icons;
};
