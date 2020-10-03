#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include <string>
#include <vector>

class Editor : public Module 
{
public:
	Editor(Application* app, bool start_enabled = true);
	~Editor();

	bool Init();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:
	//Dock Space
	update_status ShowDockSpace(bool* p_open);
	bool CreateMainMenuBar();

	//Windows
	void ShowConfigurationWindow();

	void ChangeTheme(std::string theme);

private:
	bool show_inspector_window;
	bool show_project_window;
	bool show_console_window;
	bool show_scene_window;
	bool show_configuration_window;

	//edit subwindows
	bool show_preferences_window;

	//menus
	bool* open_dockspace;

	int current_theme;

	std::vector<float> fps_log;
	std::vector<float> ms_log;
};

#endif // !_EDITOR_H_

