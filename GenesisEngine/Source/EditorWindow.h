#pragma once

enum WindowType {
	HIERARCHY_WINDOW,
	INSPECTOR_WINDOW,
	SCENE_WINDOW,
	//CONSOLE_WINDOW,
	ASSETS_WINDOW,
	CONFIGURATION_WINDOW,
	ABOUT_WINDOW,
	MAX_WINDOWS
};

class EditorWindow {
public:
	EditorWindow() : visible(false){};
	virtual ~EditorWindow() {};
	virtual void Draw() {};
public:
	bool visible;
	WindowType type;
};
