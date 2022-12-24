#include "EditorWindow_Console.h"
#include <ImGui/imgui.h>

EditorWindow_Console::EditorWindow_Console(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor)
{
	isOpen = true;
	name = "Console";
}

void EditorWindow_Console::Draw()
{
	if (ImGui::Begin(name, &isOpen)) {

	}
	ImGui::End();
}
