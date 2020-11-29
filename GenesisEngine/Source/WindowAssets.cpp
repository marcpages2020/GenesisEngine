#include "WindowAssets.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "FileSystem.h"
#include "Importers.h"

#include <algorithm>

WindowAssets::WindowAssets() : EditorWindow()
{
	type = WindowType::ASSETS_WINDOW;
	current_folder = "Assets";
}

WindowAssets::~WindowAssets() {}

void WindowAssets::Draw()
{
	if (ImGui::Begin("Assets", &visible)) {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
        ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.15f, ImGui::GetContentRegionAvail().y), false, window_flags);
		ImGui::Text("Assets");
		DrawDirectoryRecursive("Assets", nullptr);
        ImGui::EndChild();

		ImGui::SameLine();
        ImGui::BeginChild("Folder", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
		DrawCurrentFolder();
        ImGui::EndChild();
	}
	ImGui::End();
}

void WindowAssets::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;
	FileSystem::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str(), tree_flags))
		{
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::Button("Delete"))
				{
					//gameObject->to_delete = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::IsItemClicked()) {
				current_folder = directory;
				current_folder.append("/");
				current_folder.append(*it).c_str();
			}

			DrawDirectoryRecursive((dir + (*it)).c_str(), filter_extension);

			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool ok = true;

		if (filter_extension && str.substr(str.find_last_of(".") + 1) != filter_extension)
			ok = false;

		if (it->find(".meta") != -1)
			ok = false;

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::Button("Delete"))
				{
					std::string file_to_delete = directory;
					file_to_delete.append("/" + str);
					App->resources->DeleteAssetsResource(file_to_delete.c_str());
					ImGui::CloseCurrentPopup();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}
}

void WindowAssets::DrawCurrentFolder()
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	FileSystem::DiscoverFiles(current_folder.c_str(), files, dirs);

	for (size_t i = 0; i < dirs.size(); i++)
	{
		ImGui::PushID(i);
		if (ImGui::Button(dirs[i].c_str(), ImVec2(100, 100))) {
			current_folder.append("/");
			current_folder.append(dirs[i].c_str());
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsMouseDoubleClicked(0))
		{
			current_folder = current_folder + dirs[i];
		}

		ImGui::PopID();
		ImGui::SameLine();
	}

	for (size_t i = 0; i < files.size(); i++)
	{
		if (files[i].find(".meta") != -1)
			continue;

		ImGui::PushID(i);
		ImGui::Button(files[i].c_str(), ImVec2(120, 120));

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ASSETS", &i, sizeof(int));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Button("Delete"))
			{
				std::string file_to_delete = current_folder + "/" + files[i];
				App->resources->DeleteAssetsResource(file_to_delete.c_str());
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("->"))
			ImGui::OpenPopup("Meshes");
		if (ImGui::BeginPopup("Meshes"))
		{
			std::string model = current_folder + "/" + files[i];
			const char* library_path = App->resources->Find(App->resources->GetUIDFromMeta(model.append(".meta").c_str()));

			std::vector<uint> meshes;
			std::vector<uint> materials;
			ModelImporter::ExtractInternalResources(library_path, meshes, materials);

			for (size_t m = 0; m < meshes.size(); m++)
			{
				ImGui::PushID(meshes[m]);
				ImGui::Text("%d", meshes[m]);
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("MESHES", &(meshes[m]), sizeof(int));
					ImGui::EndDragDropSource();
				}
				ImGui::PopID();
			}
			ImGui::EndPopup();
		}

		ImGui::PopID();
		ImGui::SameLine();
	}

	return;
}

const char* WindowAssets::GetFileAt(int i)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	FileSystem::DiscoverFiles(current_folder.c_str(), files, dirs);
	std::string file = current_folder + "/"+ files[i];
	char* file_cstr = new char[256];
	sprintf_s(file_cstr, 256,"%s",file.c_str());

	return file_cstr;
}
