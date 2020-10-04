#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include <string>
#include <vector>

typedef int GLint;

class Editor : public Module 
{
public:
	Editor(bool start_enabled = true);
	~Editor();

	bool Init();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void AddConsoleLog(const char* log);
	
private:
	//Dock Space
	update_status ShowDockSpace(bool* p_open);
	bool CreateMainMenuBar();

	//Windows
	void ShowConfigurationWindow();
	void ShowAboutWindow();

	void ChangeTheme(std::string theme);
	void GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved);

private:
	bool show_inspector_window;
	bool show_project_window;
	bool show_hierachy_window;
	bool show_console_window;
	bool show_scene_window;
	bool show_configuration_window;
	bool show_about_window;

	//edit subwindows
	bool show_preferences_window;

	//menus
	bool* open_dockspace;

	int current_theme;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	std::vector<std::string> console_log;
};

#endif // !_EDITOR_H_

