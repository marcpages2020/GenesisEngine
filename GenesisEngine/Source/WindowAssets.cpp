#include "WindowAssets.h"
#include "ImGui/imgui.h"

WindowAssets::WindowAssets() : EditorWindow()
{
	type = WindowType::ASSETS_WINDOW;
}

WindowAssets::~WindowAssets()
{
}

void WindowAssets::Draw()
{
	ImGui::Begin("Assets", &visible);
	ImGui::End();
}
