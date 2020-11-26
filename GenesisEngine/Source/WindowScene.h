#pragma once

#include "EditorWindow.h"

class WindowScene : public EditorWindow {
public:
	WindowScene();
	~WindowScene();

	void Draw() override;
	void DrawGameTimeDataOverlay();

};
