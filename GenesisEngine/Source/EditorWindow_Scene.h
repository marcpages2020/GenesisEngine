#pragma once
#include "EditorWindow.h"

class EditorWindow_Scene : public EditorWindow
{
public:
	EditorWindow_Scene(ModuleEditor* moduleEditor);

	void Draw() override;
};

