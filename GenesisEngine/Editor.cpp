#include "Globals.h"
#include "Editor.h"

/*
#include "External Libraries/GL/gl3w.h"

#include "External Libraries/ImGui/imgui.h"
#include "External Libraries/ImGui/imgui_impl_sdl.h"
#include "External Libraries/ImGui/imgui_impl_opengl3.h"
*/

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

update_status Editor::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool Editor::CleanUp()
{
	return true;
}