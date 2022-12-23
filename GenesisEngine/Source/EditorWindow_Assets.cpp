#include "EditorWindow_Assets.h"
#include <ImGui/imgui.h>

EditorWindow_Assets::EditorWindow_Assets() : EditorWindow()
{
	isOpen = true;
	windowType = EditorWindowType::ASSETS;
}

void EditorWindow_Assets::Draw()
{
	ImGui::Begin("Assets", &isOpen);
	ImGui::End();
}
