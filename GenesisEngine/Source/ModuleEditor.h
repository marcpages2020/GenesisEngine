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
	bool Start();
	update_status Update(float deltaTime);
	update_status PostUpdate(float deltaTime);
	bool CleanUp();

	EditorWindow* GetWindowByName(const char* windowName);

private:
	update_status ShowDockSpace(bool* p_open);
	void ChangeTheme(std::string theme);

private:
	std::vector<EditorWindow*> windows;

	//menus
	bool isDockspaceOpen;
	bool showImGuiDemo;
};

#endif // !_EDITOR_H_

