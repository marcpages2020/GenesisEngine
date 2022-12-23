#pragma once
#include "EditorWindow.h"

#include <map>
#include <vector>
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

private:
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);
	void DrawCurrentFolder();
	bool DrawIcon(const char* path, int id, bool isFolder = false);
	void DrawModelInternalResources(const char* file_name);
	void DrawPathButtons();

	void LoadPreviews(std::vector<std::string> current_folder_files);
	void UnloadPreviews();

private:
	std::string current_folder;
	AssetsIcons icons;
	char selectedItem[256];
	std::map<std::string, ResourceTexture*> previews;
};
