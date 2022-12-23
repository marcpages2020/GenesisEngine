#include "EditorWindow_Inspector.h"
#include <ImGui/imgui.h>

EditorWindow_Inspector::EditorWindow_Inspector() : EditorWindow()
{
	isOpen = true;
	name = "Inspector";
}

void EditorWindow_Inspector::Draw()
{
	ImGui::Begin(name, &isOpen);
	ImGui::End();
}
