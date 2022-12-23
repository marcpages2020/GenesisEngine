#include "EditorWindow_Configuration.h"
#include <ImGui/imgui.h>

EditorWindow_Configuration::EditorWindow_Configuration()
{
	name = "Configuration";
	isOpen = true;
}

void EditorWindow_Configuration::Draw()
{
	if (ImGui::Begin(name, &isOpen))
	{

	}
	ImGui::End();
}
