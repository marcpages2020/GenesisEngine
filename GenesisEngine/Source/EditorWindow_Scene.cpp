#include "EditorWindow_Scene.h"
#include "ModuleEditor.h"
#include "Engine.h"
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
		ImGui::Image((ImTextureID)editor->engine->renderer3D->GetFinalRenderAttachmentHandle(), ImGui::GetContentRegionAvail());
	}
	ImGui::End();
}
