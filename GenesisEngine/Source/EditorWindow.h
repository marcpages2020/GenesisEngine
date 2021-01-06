#pragma once

enum WindowType {
	WINDOW_HIERARCHY,
	WINDOW_INSPECTOR,
	WINDOW_SCENE,
	WINDOW_ASSETS,
	WINDOW_CONFIGURATION,
	WINDOW_ABOUT,
	WINDOW_IMPORT,
	WINDOW_SHADER_EDITOR,
	MAX_WINDOWS
};

class EditorWindow {
public:
	EditorWindow() : visible(false){};
	virtual ~EditorWindow() {};
	virtual bool Init() { return true; };
	virtual void Draw() = 0;

public:
	bool visible;
	bool focused = false;
	WindowType type = MAX_WINDOWS;
};
