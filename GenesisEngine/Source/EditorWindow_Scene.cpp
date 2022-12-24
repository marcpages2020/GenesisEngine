#include "EditorWindow_Scene.h"
#include "EditorWindow.h"
#include <ImGui/imgui.h>

EditorWindow_Scene::EditorWindow_Scene(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor)
{
	name = "Scene";
	isOpen = true;
}

void EditorWindow_Scene::Draw()
{
	if (ImGui::Begin(name, &isOpen))
	{
		ImGui::Image(ImTextureID(), ImGui::GetContentRegionAvail());
	}
	ImGui::End();
}
