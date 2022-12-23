#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include <stdio.h>
#include <string>
#include <vector>

class EditorWindow;

class ModuleEditor : public Module 
{
public:
	ModuleEditor(GnEngine* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:
	update_status ShowDockSpace(bool* p_open);
	void ChangeTheme(std::string theme);

private:
	std::vector<EditorWindow*> windows;

	bool show_inspector_window;
	bool show_project_window;
	bool show_console_window;

	//edit subwindows
	bool show_preferences_window;

	//menus
	bool* open_dockspace;

	int current_theme;
};

#endif // !_EDITOR_H_

