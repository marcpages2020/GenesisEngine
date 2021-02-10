#pragma once

enum WindowType {
	WINDOW_ABOUT,
	WINDOW_ASSETS,
	WINDOW_CONFIGURATION,
	WINDOW_CONSOLE,
	WINDOW_HIERARCHY,
	WINDOW_IMPORT,
	WINDOW_INSPECTOR,
	WINDOW_SCENE,
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
