#pragma once
#include "EditorWindow.h"

enum class RenderingMode
{
	ALBEDO,
	NORMALS,
	POSITION,
	DEPTH,
	METALLIC,
	FINAL_RENDER
};

class EditorWindow_Scene : public EditorWindow
{
public:
	EditorWindow_Scene(ModuleEditor* moduleEditor);

	void Draw() override;

private: 
	RenderingMode currentRenderingMode;
};

