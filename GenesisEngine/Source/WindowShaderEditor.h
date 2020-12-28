#ifndef _WINDOW_SHADER_EDITOR_H_
#define _WINDOW_SHADER_EDITOR_H_

#include "EditorWindow.h"
#include "TextEditor/TextEditor.h"

class WindowShaderEditor : public EditorWindow {
public:
	WindowShaderEditor();
	~WindowShaderEditor();

	void Open(const char* assets_file_path);
	void Draw() override;
	void DrawEditor(TextEditor& editor, char* path);

private:
	char vertexShaderPath[128];
	char fragmentShaderPath[128];

	TextEditor vertexShaderEditor;
	TextEditor fragmentShaderEditor;
};

#endif // !_WINDOW_SHADER_EDITOR_H_

