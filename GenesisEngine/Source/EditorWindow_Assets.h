#pragma once
#include "EditorWindow.h"
class EditorWindow_Assets : public EditorWindow
{
public:
	EditorWindow_Assets(ModuleEditor* moduleEditor);

	void Draw() override;
};

