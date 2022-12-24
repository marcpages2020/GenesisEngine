#include "EditorWindow_Inspector.h"
#include <ImGui/imgui.h>

EditorWindow_Inspector::EditorWindow_Inspector(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor)
{
	name = "Inspector";
	isOpen = true;
}

void EditorWindow_Inspector::Draw()
{
	if (ImGui::Begin(name, &isOpen)) 
	{

	}
	ImGui::End();
}
