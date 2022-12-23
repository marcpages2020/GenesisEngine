#pragma once

#include "EditorWindow.h"

class WindowAbout : public EditorWindow {
public:
	WindowAbout();
	~WindowAbout();
	void Draw() override;
};