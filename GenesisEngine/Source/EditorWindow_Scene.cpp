#include "EditorWindow_Scene.h"
#include <ImGui/imgui.h>

EditorWindow_Scene::EditorWindow_Scene()
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
