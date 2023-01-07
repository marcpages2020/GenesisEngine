#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "EditorWindow.h"

#include <vector>
#include <string>

typedef int GLint;

enum class EditorTheme
{
	LIGHT,
	CLASSIC,
	DARK
};

class ModuleEditor : public Module 
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	bool Start() override;
	bool LoadConfig(GnJSONObj& config) override;
	update_status Update(float deltaTime);
	update_status Draw();
	bool CleanUp();

	bool IsWindowFocused(const char* windowName);
	bool MouseOnScene();
	EditorWindow* GetWindow(const char* windowName);

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

	void ChangeTheme(EditorTheme newTheme);

public:
	ImVec2 image_size;
	ImVec2 sceneWindowOrigin;
	ImVec2 mouseScenePosition;
	std::map<const char*, EditorWindow*> windows;

private:

	void AddWindow(EditorWindow* windowToAdd);

	bool show_project_window;

	//edit subwindows
	bool show_preferences_window;
	bool show_game_buttons;

	//menus
	bool open_dockspace;

	EditorTheme currentTheme;

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

