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

	//menus
	bool isDockspaceOpen;

	int currentTheme;
};

#endif // !_EDITOR_H_

