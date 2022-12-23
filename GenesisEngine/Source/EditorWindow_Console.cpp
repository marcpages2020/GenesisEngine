#include "EditorWindow_Console.h"
#include <ImGui/imgui.h>

EditorWindow_Console::EditorWindow_Console()
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
