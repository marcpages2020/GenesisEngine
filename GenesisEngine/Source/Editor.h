#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

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
	update_status ShowDockSpace(bool* p_open);

private:
	bool show_demo_window;
	bool show_another_window;

	//menus
	bool* open_dockspace;
};

#endif // !_EDITOR_H_

