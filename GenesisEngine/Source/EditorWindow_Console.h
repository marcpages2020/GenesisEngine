#pragma once
#include "EditorWindow.h"
class EditorWindow_Console : public EditorWindow
{
public:
	EditorWindow_Console(ModuleEditor* moduleEditor);

	void Draw() override;
};

