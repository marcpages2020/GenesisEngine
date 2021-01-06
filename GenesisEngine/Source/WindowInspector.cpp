#include "WindowInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

WindowInspector::WindowInspector() : EditorWindow()
{
	type = WindowType::WINDOW_INSPECTOR;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::Draw()
{
	if (ImGui::Begin("Inspector", &visible))
	{
		focused = ImGui::IsWindowFocused();
		if (App->scene->selectedGameObject != nullptr)
		{
			App->scene->selectedGameObject->OnEditor();
		}

		if (ImGui::CollapsingHeader("Resources")) {
			App->resources->OnEditor();
		}
	}
	ImGui::End();
}
