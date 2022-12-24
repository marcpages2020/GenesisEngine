#pragma once
#include "EditorWindow.h"
class EditorWindow_Inspector : public EditorWindow
{
public: 
	EditorWindow_Inspector(ModuleEditor* moduleEditor);

	void Draw() override;
};

