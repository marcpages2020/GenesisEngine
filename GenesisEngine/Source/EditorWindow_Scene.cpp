#include "EditorWindow_Scene.h"
#include "ModuleEditor.h"
#include "Engine.h"
#include <ImGui/imgui.h>

EditorWindow_Scene::EditorWindow_Scene(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor),
currentRenderingMode(RenderingMode::FINAL_RENDER)
{
	name = "Scene";
	isOpen = true;
}

void EditorWindow_Scene::Draw()
{
	if (ImGui::Begin(name, &isOpen, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			const char* renderingModes[] = { "Albedo", "Normals", "Position", "Depth", "Metallic", "Final Render" };
			int renderingMode = (int)currentRenderingMode;
			if (ImGui::Combo("Rendering Mode", &renderingMode, renderingModes, IM_ARRAYSIZE(renderingModes)))
			{
				currentRenderingMode = (RenderingMode)renderingMode;
			}

			ImGui::EndMenuBar();
		}

		ImTextureID image;
		switch (currentRenderingMode)
		{
		case RenderingMode::ALBEDO: image = (ImTextureID)editor->engine->renderer3D->GetAlbedoAttachmentHandle();  break;
		case RenderingMode::NORMALS: image = (ImTextureID)editor->engine->renderer3D->GetNormalsAttachmentHandle(); break;
		case RenderingMode::POSITION: image = (ImTextureID)editor->engine->renderer3D->GetPositionAttachmentHandle(); break;
		case RenderingMode::DEPTH: image = (ImTextureID)editor->engine->renderer3D->GetDepthAttachmentHandle(); break;
		case RenderingMode::METALLIC: image = (ImTextureID)editor->engine->renderer3D->GetMetallicAttachmentHandle(); break;
		case RenderingMode::FINAL_RENDER: image = (ImTextureID)editor->engine->renderer3D->GetFinalRenderAttachmentHandle(); break;
		default: image = (ImTextureID)editor->engine->renderer3D->GetFinalRenderAttachmentHandle(); break;
		}
		
		ImGui::Image(image, ImGui::GetContentRegionAvail());
	}
	ImGui::End();
}
