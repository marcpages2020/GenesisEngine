#include "WindowShaderEditor.h"
#include "FileSystem.h"
#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "ImGui/imgui.h"

WindowShaderEditor::WindowShaderEditor() : EditorWindow()
{
	type = WindowType::SHADER_EDITOR_WINDOW;
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
	std::string fragmentShaderPath = ShaderImporter::FindPairingShader(assets_file_path);
	FileSystem::Load(fragmentShaderPath.c_str(), &secondaryBuffer);

	if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::VERTEX_SHADER)
	{
		strcpy(vertexShaderBuffer, buffer);
		strcpy(fragmentShaderBuffer, secondaryBuffer);
	}
	else if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::FRAGMENT_SHADER)
	{
		strcpy(vertexShaderBuffer, secondaryBuffer);
		strcpy(fragmentShaderBuffer, buffer);
	}

	RELEASE_ARRAY(buffer);
	RELEASE_ARRAY(secondaryBuffer);

	visible = true;
}

void WindowShaderEditor::Draw()
{
	if (ImGui::Begin("Shader Editor", &visible))
	{
		if (ImGui::BeginTabBar("##TabBar"))
		{
			if (ImGui::BeginTabItem("Vertex Shader"))
			{
				static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
				ImGui::InputTextMultiline("##source", vertexShaderBuffer, 2048, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 24), flags);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Fragment Shader"))
			{
				static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
				ImGui::InputTextMultiline("##source", fragmentShaderBuffer, 2048, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
				ImGui::EndTabItem();
			}

			ImGui::Button("Compile");
			ImGui::SameLine();
			ImGui::Button("Compile and Close");
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				visible = false;

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}
