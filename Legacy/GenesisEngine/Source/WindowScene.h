#pragma once

#include "EditorWindow.h"

class ResourceShader;

class WindowScene : public EditorWindow {
public:
	WindowScene();
	~WindowScene();

	bool Init() override;
	void Draw() override;
	void DrawGameTimeDataOverlay();
	void ApplyDroppedFile(const char* assets_file);

private:
	ResourceShader* shader;
};
