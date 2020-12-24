#pragma once

enum WindowType {
	HIERARCHY_WINDOW,
	INSPECTOR_WINDOW,
	SCENE_WINDOW,
	ASSETS_WINDOW,
	CONFIGURATION_WINDOW,
	ABOUT_WINDOW,
	IMPORT_WINDOW,
	SHADER_EDITOR_WINDOW,
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
	WindowType type = MAX_WINDOWS;

	char vertexShaderBuffer[2048];
	char fragmentShaderBuffer[2048];
};
