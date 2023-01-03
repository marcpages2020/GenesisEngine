#include "WindowInspector.h"
#include "Engine.h"
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
		if (engine->scene->selectedGameObject != nullptr)
		{
			engine->scene->selectedGameObject->OnEditor();
		}

		if (ImGui::CollapsingHeader("Resources")) {
			engine->resources->OnEditor();
		}
	}
	ImGui::End();
}
