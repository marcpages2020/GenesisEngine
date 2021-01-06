#include "WindowShaderEditor.h"
#include "FileSystem.h"
#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "Application.h"
#include <utility>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"


WindowShaderEditor::WindowShaderEditor() : EditorWindow(), text_size(14)
{
	type = WindowType::WINDOW_SHADER_EDITOR;
}

WindowShaderEditor::~WindowShaderEditor()
{}

bool WindowShaderEditor::Init()
{
	bool ret = true;

	auto lang = TextEditor::LanguageDefinition::GLSL();

	static const char* identifiers[] = 
	{
		 "model_matrix" 
	};
	static const char* idecls[] =
	{
	 "object transform" 
	};

	for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
	{
		TextEditor::Identifier id;
		id.mDeclaration = std::string(idecls[i]);
		lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
	}

	vertexShaderEditor.SetLanguageDefinition(lang);
	fragmentShaderEditor.SetLanguageDefinition(lang);

	return ret;
}

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
		focused = ImGui::IsWindowFocused();
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
				//clear errors
				TextEditor::ErrorMarkers markers;
				vertexShaderEditor.SetErrorMarkers(markers);
				fragmentShaderEditor.SetErrorMarkers(markers);

				FileSystem::Save(vertexShaderPath, vertexShaderEditor.GetText().c_str(), strlen(vertexShaderEditor.GetText().c_str()));
				FileSystem::Save(fragmentShaderPath, fragmentShaderEditor.GetText().c_str(), strlen(fragmentShaderEditor.GetText().c_str()));
				
				ShaderImporter::RecompileShader(vertexShaderPath, fragmentShaderPath);

				App->resources->SaveResource(App->resources->GetResource(App->resources->Find(vertexShaderPath)));
			}

			ImGui::SameLine();
			if (ImGui::Button("Compile and Close")) 
			{
				//clear errors
				TextEditor::ErrorMarkers markers;
				vertexShaderEditor.SetErrorMarkers(markers);
				fragmentShaderEditor.SetErrorMarkers(markers);

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

void WindowShaderEditor::SetErrorsOnScreen(const char* infoLog, ShaderType type)
{
	TextEditor::ErrorMarkers markers;

	std::vector<std::pair<int, std::string>> errors = SplitErrors(infoLog);

	for (size_t i = 0; i < errors.size(); i++)
	{
		markers.insert(errors[i]);
	}

	if (type == ShaderType::VERTEX_SHADER)
		vertexShaderEditor.SetErrorMarkers(markers);
	else
		fragmentShaderEditor.SetErrorMarkers(markers);
}

std::vector<std::pair<int, std::string>> WindowShaderEditor::SplitErrors(const char* infoLog)
{
	std::vector<std::pair<int, std::string>> errors;

	std::string tmp = infoLog;

	while (tmp.find("0(") != std::string::npos)
	{
		size_t error_line_pos = tmp.find("0(");
		std::string error_line = tmp.substr(error_line_pos + 2, 2);

		std::pair<int, std::string> error;
		error.first = std::stoi(error_line);

		size_t found = tmp.find("\n");
		if (found != std::string::npos)
		{
			std::string error_text = tmp.substr(7, found - 7);

			error.second = error_text;

			tmp.erase(tmp.begin(), tmp.begin() + found + 1);
		}
		else
		{
			error.second = tmp.substr(0);
			tmp.erase(tmp.begin(), tmp.begin() + found + 1);
		}

		errors.push_back(error);
	}

	return errors;
}
