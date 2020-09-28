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
	update_status PreUpdate(float dt);
	bool CleanUp();
};

#endif // !_EDITOR_H_

