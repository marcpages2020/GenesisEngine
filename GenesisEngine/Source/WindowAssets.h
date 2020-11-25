#pragma once
#include "EditorWindow.h"

#include <string>

class WindowAssets : public EditorWindow {
public:
	WindowAssets();
	~WindowAssets();

	void Draw() override;
	const char* GetFileAt(int i);

private:
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);
	void DrawCurrentFolder();

private:
	std::string current_folder;
};
