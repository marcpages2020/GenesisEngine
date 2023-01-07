#include "WindowHierarchy.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Engine.h"

WindowHierarchy::WindowHierarchy() : EditorWindow() 
{
	name = "Hierarchy";
}

WindowHierarchy::~WindowHierarchy() {}

void WindowHierarchy::Draw()
{
	if (ImGui::Begin(name, &visible))
	{
		focused = ImGui::IsWindowFocused();
		GameObject* root = engine->scene->GetRoot();
		int id = 0;
		PreorderHierarchy(root, id);
	}
	ImGui::End();
}

void WindowHierarchy::PreorderHierarchy(GameObject* gameObject, int& id)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (gameObject == engine->scene->GetRoot())
		flags |= ImGuiTreeNodeFlags_DefaultOpen;

	if (engine->scene->selectedGameObject == gameObject)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (gameObject->GetChildrenAmount() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;


	if (ImGui::TreeNodeEx(gameObject->GetName(), flags))
	{
		if (ImGui::IsItemClicked())
			engine->scene->selectedGameObject = gameObject;

		ImGui::PushID(gameObject->UUID);
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("HIERARCHY", &gameObject->UUID, sizeof(int));
			ImGui::Text("Reparent");
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;

				std::vector<GameObject*> gameObjects = engine->scene->GetAllGameObjects();
				GameObject* target = nullptr;

				for (size_t i = 0; i < gameObjects.size(); i++)
				{
					if (gameObjects[i]->UUID == payload_n) {
						target = gameObjects[i];
						break;
					}
				}

				target->Reparent(gameObject);
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
		id++;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Button("Add Empty Child"))
			{
				gameObject->AddChild(new GameObject());
				ImGui::CloseCurrentPopup();
			}
			else if (ImGui::Button("Delete"))
			{
				gameObject->to_delete = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		for (size_t i = 0; i < gameObject->GetChildrenAmount(); i++)
		{
			PreorderHierarchy(gameObject->GetChildAt(i), id);
		}
		ImGui::TreePop();
	}
}