#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "EditorWindow.h"

#include <vector>
#include <string>

typedef int GLint;

struct log_message {
	std::string log_text;
	int warning_level;
};

class Editor : public Module 
{
public:
	Editor(bool start_enabled = true);
	~Editor();

	bool Init();
	bool Start() override;
	bool LoadConfig(GnJSONObj& config) override;
	update_status Update(float dt);
	update_status Draw();
	bool CleanUp();

	bool IsSceneFocused();
	bool MouseOnScene();
	void AddConsoleLog(const char* log, int warning_level);

	void OnResize(ImVec2 window_size);
	void LoadFile(const char* filter_extension, const char* from_dir);
	void SaveFile(const char* filter_extension, const char* from_dir);
	void DrawDirectoryRecursive(const char* directory, const char* filter_extension);

private:
	//Dock Space
	update_status ShowDockSpace(bool* p_open);
	bool CreateMainMenuBar();

	void ShowGameButtons();

	//Windows
	void ShowPreferencesWindow();

	void ChangeTheme(std::string theme);

public:
	ImVec2 image_size;
	ImVec2 sceneWindowOrigin;
	ImVec2 mouseScenePosition;
	bool scene_window_focused;
	EditorWindow* windows[MAX_WINDOWS];

private:

	bool show_project_window;
	bool show_console_window;

	//edit subwindows
	bool show_preferences_window;
	bool show_game_buttons;

	//menus
	bool* open_dockspace;

	int current_theme;

	std::vector<log_message> console_log;

	enum
	{
		closed,
		opened,
		ready_to_close
	} file_dialog = closed;

	enum class SceneOperation
	{
		SAVE,
		LOAD, 
		NONE
	}scene_operation = SceneOperation::NONE;

	bool in_modal = false;
	char selected_folder[256];
	char scene_name[128];
	char selected_file[256];
};

#endif // !_EDITOR_H_

