#include "WindowScene.h"
#include "ImGui/imgui.h"
#include "Engine.h"
#include "glad/include/glad/glad.h"
#include "WindowAssets.h"
#include "Time.h"
#include "GameObject.h"
#include "Material.h"
#include "ResourceShader.h"

WindowScene::WindowScene() : EditorWindow()
{
	name = "Scene";
}

WindowScene::~WindowScene() {}

bool WindowScene::Init()
{
	return true;
}

void WindowScene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin(name, &visible, ImGuiWindowFlags_MenuBar))
	{
		focused = ImGui::IsWindowFocused();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Display"))
			{
				if (ImGui::BeginMenu("Shading Mode"))
				{
					if (ImGui::MenuItem("Solid", NULL, engine->renderer3D->display_mode == DisplayMode::SOLID))
						engine->renderer3D->SetDisplayMode(DisplayMode::SOLID);
					if (ImGui::MenuItem("Wireframe", NULL, engine->renderer3D->display_mode == DisplayMode::WIREFRAME))
						engine->renderer3D->SetDisplayMode(DisplayMode::WIREFRAME);
					ImGui::EndMenu();
				}

				ImGui::Checkbox("Draw Mouse Picking Ray", &engine->renderer3D->draw_mouse_picking_ray);

				ImGui::Checkbox("Draw AABBs", &engine->renderer3D->draw_aabbs);

				ImGui::Checkbox("Vertex Normals", &engine->renderer3D->draw_vertex_normals);

				ImGui::Checkbox("Face Normals", &engine->renderer3D->draw_face_normals);

				ImGui::EndMenu();
			}

			static bool lighting = glIsEnabled(GL_LIGHTING);
			if (ImGui::Checkbox("Lighting", &lighting))
				engine->renderer3D->SetCapActive(GL_LIGHTING, lighting);

			static bool show_grid = engine->scene->show_grid;
			if (ImGui::Checkbox("Show Grid", &show_grid))
				engine->scene->show_grid = show_grid;

			ImGui::Checkbox("Cull editor camera", &engine->renderer3D->cull_editor_camera);

			ImGui::EndMenuBar();
		}

		ImVec2 window_size = ImGui::GetContentRegionAvail();
		engine->editor->sceneWindowOrigin = ImGui::GetWindowPos();
		engine->editor->sceneWindowOrigin.x += ImGui::GetWindowContentRegionMin().x;
		engine->editor->sceneWindowOrigin.y += ImGui::GetWindowContentRegionMin().y;

		engine->editor->mouseScenePosition.x = engine->input->GetMouseX() - engine->editor->sceneWindowOrigin.x;
		engine->editor->mouseScenePosition.y = engine->input->GetMouseY() - engine->editor->sceneWindowOrigin.y;

		if (engine->in_game)
		{
			DrawGameTimeDataOverlay();
		}

		if (engine->editor->image_size.x != window_size.x || engine->editor->image_size.y != window_size.y)
		{
			engine->editor->OnResize(window_size);
		}

		ImGui::Image((ImTextureID)engine->renderer3D->finalRenderAttachmentHandle, window_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		ImGui::PushID("Scene");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				WindowAssets* assets_window = (WindowAssets*)engine->editor->GetWindow("Scene");
				std::string file = engine->resources->FindAsset(payload_n);
				ApplyDroppedFile(file.c_str());
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		engine->scene->EditTransform();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void WindowScene::DrawGameTimeDataOverlay()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	window_flags |= ImGuiWindowFlags_NoMove;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 window_pos = engine->editor->sceneWindowOrigin;
	window_pos.x += 10.0f;
	window_pos.y += 10.0f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
	ImGui::SetNextWindowPos(window_pos);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.25f, 0.25f, 0.25f, 0.75f));
	bool dummy_bool = true;
	if (ImGui::Begin("Example: Simple overlay", &dummy_bool, window_flags))
	{
		ImGui::Text("Game Time");
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Delta time %.3f", Time::gameClock.deltaTime);
		ImGui::Text("Time Scale: %.2f", Time::gameClock.timeScale);
		ImGui::Text("Time since game start: %.2f", Time::gameClock.timeSinceStartup());
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();
}

void WindowScene::ApplyDroppedFile(const char* assets_file)
{
	ResourceType type = engine->resources->GetTypeFromPath(assets_file);
	GameObject* selected_object = engine->scene->selectedGameObject;

	if (type == ResourceType::RESOURCE_MODEL)
	{
		engine->scene->AddGameObject(engine->resources->RequestGameObject(assets_file));
	}
	else if (type == ResourceType::RESOURCE_TEXTURE)
	{

		if (selected_object == nullptr)
			return;

		Material* material = (Material*)selected_object->GetComponent(ComponentType::MATERIAL);
		if (material != nullptr)
		{
			material->SetTexture((ResourceTexture*)engine->resources->RequestResource(engine->resources->Find(assets_file)));}
	}
	else if (type == ResourceType::RESOURCE_SHADER)
	{
		Material* material = (Material*)selected_object->GetComponent(ComponentType::MATERIAL);
		if (material != nullptr)
		{
			material->SetShader((ResourceShader*)engine->resources->RequestResource(engine->resources->Find(assets_file)));}
	}
}
