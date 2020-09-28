#include "Globals.h"
#include "Editor.h"

Editor::Editor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

Editor::~Editor() {

}

bool Editor::Init() 
{
	return true;
}

update_status Editor::PreUpdate(float dt) 
{
	return UPDATE_CONTINUE;
}

bool Editor::CleanUp()
{
	return true;
}