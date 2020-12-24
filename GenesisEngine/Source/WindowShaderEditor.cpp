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
	std::string pairingShaderPath = ShaderImporter::FindPairingShader(assets_file_path);
	FileSystem::Load(pairingShaderPath.c_str(), &secondaryBuffer);

	if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::VERTEX_SHADER)
	{
		strcpy_s(vertexShaderBuffer, buffer);
		strcpy_s(fragmentShaderBuffer, secondaryBuffer);

		strcpy_s(vertexShaderPath, assets_file_path);
		strcpy_s(fragmentShaderPath, pairingShaderPath.c_str());
	}
	else if (ShaderImporter::GetTypeFromPath(assets_file_path) == ShaderType::FRAGMENT_SHADER)
	{
		strcpy_s(vertexShaderBuffer, secondaryBuffer);
		strcpy_s(fragmentShaderBuffer, buffer);

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
	if (ImGui::Begin("Shader Editor", &visible))
	{
		if (ImGui::BeginTabBar("##TabBar"))
		{
			if (ImGui::BeginTabItem("Vertex Shader"))
			{
				static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
				ImGui::InputTextMultiline("##source", vertexShaderBuffer, IM_ARRAYSIZE(vertexShaderBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 24), flags);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Fragment Shader"))
			{
				static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CtrlEnterForNewLine;
				ImGui::InputTextMultiline("##source", fragmentShaderBuffer, IM_ARRAYSIZE(fragmentShaderBuffer), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
				ImGui::EndTabItem();
			}

			if (ImGui::Button("Compile")) {
				FileSystem::Save(vertexShaderPath, vertexShaderBuffer, strlen(vertexShaderBuffer));
				FileSystem::Save(fragmentShaderPath, fragmentShaderBuffer, strlen(fragmentShaderBuffer));
				
				ShaderImporter::RecompileShader(vertexShaderPath, fragmentShaderPath);
			}

			ImGui::SameLine();
			if (ImGui::Button("Compile and Close")) {
				FileSystem::Save(vertexShaderPath, vertexShaderBuffer, strlen(vertexShaderBuffer));
				FileSystem::Save(fragmentShaderPath, fragmentShaderBuffer, strlen(fragmentShaderBuffer));

				ShaderImporter::RecompileShader(vertexShaderPath, fragmentShaderPath);

				visible = false;
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				visible = false;

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}
