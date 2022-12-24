#pragma once
#include "EditorWindow.h"
#include <vector>

class EditorWindow_Configuration : public EditorWindow
{
public:
	EditorWindow_Configuration(ModuleEditor* moduleEditor);

	void Draw() override;

private: 
	std::vector<float> FPSLog;
	std::vector<float> msLog;
};

