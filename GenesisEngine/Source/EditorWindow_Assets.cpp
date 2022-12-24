#include "EditorWindow_Assets.h"
#include <ImGui/imgui.h>

EditorWindow_Assets::EditorWindow_Assets(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor)
{
	name = "Assets";
	isOpen = true;
}

void EditorWindow_Assets::Draw()
{
	if (ImGui::Begin(name, &isOpen)) {
	}
	ImGui::End();
}
