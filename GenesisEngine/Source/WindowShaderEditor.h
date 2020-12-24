#ifndef _WINDOW_SHADER_EDITOR_H_
#define _WINDOW_SHADER_EDITOR_H_

#include "EditorWindow.h"

class WindowShaderEditor : public EditorWindow {
public:
	WindowShaderEditor();
	~WindowShaderEditor();

	void Open(const char* assets_file_path);
	void Draw() override;

private:
	char vertexShaderBuffer[2048];
	char fragmentShaderBuffer[2048];

	char vertexShaderPath[128];
	char fragmentShaderPath[128];
};

#endif // !_WINDOW_SHADER_EDITOR_H_

