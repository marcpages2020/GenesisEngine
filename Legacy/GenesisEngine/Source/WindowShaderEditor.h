#ifndef _WINDOW_SHADER_EDITOR_H_
#define _WINDOW_SHADER_EDITOR_H_

#include "EditorWindow.h"
#include "TextEditor/TextEditor.h"

enum class ShaderType;

class WindowShaderEditor : public EditorWindow {
public:
	WindowShaderEditor();
	~WindowShaderEditor();

	bool Init();
	void Open(const char* assets_file_path);
	void Draw() override;
	void DrawEditor(TextEditor& editor, char* path);
	void SetErrorsOnScreen(const char* infoLog, ShaderType type);
	std::vector<std::pair<int, std::string>> SplitErrors(const char* infoLog);

private:
	int text_size;
	bool vertex_shader_has_error;
	bool fragment_shader_has_error;

	char vertexShaderPath[128];
	char fragmentShaderPath[128];

	TextEditor vertexShaderEditor;
	TextEditor fragmentShaderEditor;
};

#endif // !_WINDOW_SHADER_EDITOR_H_

