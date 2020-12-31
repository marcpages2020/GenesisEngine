#include "WindowShaderEditor.h"
#include "FileSystem.h"
#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "Application.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"


WindowShaderEditor::WindowShaderEditor() : EditorWindow()
{
	type = WindowType::SHADER_EDITOR_WINDOW;

	auto lang = TextEditor::LanguageDefinition::GLSL();
	vertexShaderEditor.SetLanguageDefinition(lang);
	fragmentShaderEditor.SetLanguageDefinition(lang);
}

WindowShaderEditor::~WindowShaderEditor()
{}

void WindowShaderEditor::Open(const char* assets_file_path)
{
	char* buffer;
	char* secondaryBuffer;

	//load given shader
	FileSystem::Load(assets_file_path, &buffer);

	//load pairing shader
	std::string pairingShaderPath = ShaderImporter::FindPairingShader(assets_file_path);
	FileSystem::Load(pairingShaderPath.c_str(), &secondaryBuffer);

	if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::VERTEX_SHADER)
	{
		vertexShaderEditor.SetText(buffer);
		fragmentShaderEditor.SetText(secondaryBuffer);

		strcpy_s(vertexShaderPath, assets_file_path);
		strcpy_s(fragmentShaderPath, pairingShaderPath.c_str());
	}
	else if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::FRAGMENT_SHADER)
	{
		fragmentShaderEditor.SetText(buffer);
		vertexShaderEditor.SetText(secondaryBuffer);

		strcpy_s(fragmentShaderPath, assets_file_path);
		strcpy_s(vertexShaderPath, pairingShaderPath.c_str());
	}
	else
	{
		LOG_ERROR("Should not edit unknown shader type");
	}

	RELEASE_ARRAY(buffer);
	RELEASE_ARRAY(secondaryBuffer);

	visible = true;
}

void WindowShaderEditor::Draw()
{
	if (ImGui::Begin("Shader Editor", &visible), ImGuiWindowFlags_MenuBar)
	{

		if (ImGui::BeginTabBar("##TabBar"))
		{
			//Tabs =================================================================================================

			if (ImGui::BeginTabItem("Vertex Shader"))
			{
				DrawEditor(vertexShaderEditor, vertexShaderPath);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Fragment Shader"))
			{
				DrawEditor(fragmentShaderEditor, fragmentShaderPath);

				ImGui::EndTabItem();
			}

			//ImGui::Spacing();
			ImGui::Dummy(ImVec2(10.0f, 10.0f));

			//Buttons =============================================================================================

			if (ImGui::Button("Compile"))
			{
				FileSystem::Save(vertexShaderPath, vertexShaderEditor.GetText().c_str(), strlen(vertexShaderEditor.GetText().c_str()));
				FileSystem::Save(fragmentShaderPath, fragmentShaderEditor.GetText().c_str(), strlen(fragmentShaderEditor.GetText().c_str()));
				
				ShaderImporter::RecompileShader(vertexShaderPath, fragmentShaderPath);

				App->resources->SaveResource(App->resources->GetResource(App->resources->Find(vertexShaderPath)));
			}

			ImGui::SameLine();
			if (ImGui::Button("Compile and Close")) 
			{
				FileSystem::Save(vertexShaderPath, vertexShaderEditor.GetText().c_str(), strlen(vertexShaderEditor.GetText().c_str()));
				FileSystem::Save(fragmentShaderPath, fragmentShaderEditor.GetText().c_str(), strlen(fragmentShaderEditor.GetText().c_str()));

				ShaderImporter::RecompileShader(vertexShaderPath, fragmentShaderPath);

				App->resources->SaveResource(App->resources->GetResource(App->resources->Find(vertexShaderPath)));

				visible = false;
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				visible = false;

			ImGui::EndTabBar();
		}

		ImGui::Dummy(ImVec2(10.0f, 10.0f));

		ImGui::End();
	}
}

void WindowShaderEditor::DrawEditor(TextEditor& editor, char* path)
{
	auto cpos = editor.GetCursorPosition();

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), path);

	const ImVec2 content_avail = ImGui::GetContentRegionAvail();
	ImVec2 size;

	size.x = ImMax(content_avail.x, 4.0f);
	size.y = ImMax(content_avail.y - 40.0f, 4.0f);

	editor.Render("TextEditor", size);
}
