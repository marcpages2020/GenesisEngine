#include "EditorWindow_Assets.h"
#include <ImGui/imgui.h>

EditorWindow_Assets::EditorWindow_Assets() : EditorWindow()
{
	isOpen = true;
	name = "Assets";
}

void EditorWindow_Assets::Draw()
{
	ImGui::Begin(name, &isOpen);
	ImGui::End();
}
