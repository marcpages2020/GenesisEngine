#include "WindowAssets.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "FileSystem.h"
#include "Importers.h"
#include "ResourceTexture.h"

#include <algorithm>

WindowAssets::WindowAssets() : EditorWindow()
{
	type = WindowType::ASSETS_WINDOW;
	current_folder = "Assets";
	selectedItem[0] = '\0';
}

WindowAssets::~WindowAssets() 
{
	current_folder.clear();
}

bool WindowAssets::Init()
{
	bool ret = true;

	App->resources->LoadEngineAssets(icons);

	return ret;
}

void WindowAssets::Draw()
{
	if (ImGui::Begin("Assets", &visible)) 
	{
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
        ImGui::BeginChild("Tree", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.15f, ImGui::GetContentRegionAvail().y), false, window_flags);
		ImGui::Text("Assets");
		DrawDirectoryRecursive("Assets", nullptr);
		ImGui::Spacing();
		if (ImGui::Button("Reload", ImVec2(50, 16)))
			App->resources->CheckAssetsRecursive("Assets");
        ImGui::EndChild();

		ImGui::SameLine();
        ImGui::BeginChild("Folder", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
		ImGui::Columns(1);
		DrawPathButtons();
		ImGui::Spacing();
		ImGui::Spacing();
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
		bool open = ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str(), tree_flags);
		
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

		if (open) 
		{
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
					App->resources->AddAssetToDelete(file_to_delete.c_str());
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
	std::vector<std::string> tmp_files;
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	FileSystem::DiscoverFiles(current_folder.c_str(), tmp_files, dirs);

	for (size_t i = 0; i < tmp_files.size(); ++i)
	{
		if (tmp_files[i].find(".meta") == std::string::npos) 
		{
			files.push_back(tmp_files[i]);
		}
	}

	tmp_files.clear();

	int total_icons = files.size() + dirs.size();
	int icons_drawn = 0;

	int columns = floor(ImGui::GetContentRegionAvailWidth() / 100.0f);
	int rows = ceil(files.size() + dirs.size() / (float)columns);

	int drawn_columns = 0;
	int drawn_rows = 0;

	int dirs_drawn = 0;
	int files_drawn = 0;

	bool ret = true;

	//if (ImGui::IsMouseClicked(0))
		//selectedItem[0] = '\0';

	for (size_t r = 0; r < rows && icons_drawn < total_icons && ret; r++)
	{
		ImGui::Columns(columns, 0, false);
		for (size_t c = 0; c < columns && icons_drawn < total_icons && ret; c++)
		{
			if (dirs_drawn < dirs.size()) 
			{
				std::string path = current_folder + "/" + dirs[dirs_drawn];
				ret = DrawIcon(path.c_str(), icons_drawn, true);
				dirs_drawn++;
			}
			else
			{
				std::string path = current_folder + "/" + files[files_drawn];
				DrawIcon(path.c_str(), icons_drawn, false);
				files_drawn++;
			}
			ImGui::NextColumn();
			icons_drawn++;
		}
		ImGui::Columns(1);
	}

	/*
	for (size_t i = 0; i < dirs.size(); i++)
	{
		ImGui::PushID(i);
		std::string path = current_folder;
		path.append(dirs[i].c_str());

		//DrawIcon(dirs[i].c_str(), true);

		
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
				App->resources->AddAssetToDelete(file_to_delete.c_str());
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (files[i].find(".fbx") != std::string::npos)
		{
			ImGui::SameLine();
			if (ImGui::Button("->"))
				ImGui::OpenPopup("Meshes");
			if (ImGui::BeginPopup("Meshes", ImGuiWindowFlags_NoMove))
			{
				std::string model = current_folder + "/" + files[i];
				const char* library_path = App->resources->Find(App->resources->GetUIDFromMeta(model.append(".meta").c_str()));

				std::vector<uint> meshes;
				std::vector<uint> materials;
				ModelImporter::ExtractInternalResources(library_path, meshes, materials);

				for (size_t m = 0; m < meshes.size(); m++)
				{
					ImGui::PushID(meshes[m]);
					ResourceData meshData = App->resources->RequestResourceData(meshes[m]);
					ImGui::Text("%s", meshData.name.c_str());
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("MESHES", &(meshes[m]), sizeof(int));
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}
				ImGui::EndPopup();
			}
		}

		ImGui::PopID();
		ImGui::SameLine();
	}
	*/
}

bool WindowAssets::DrawIcon(const char* path, int id, bool isFolder)
{
	bool ret = true;
	ImVec4 fileColor = ImVec4(0, 0, 0, 1);

	if (strcmp(selectedItem, path) == 0)
		fileColor = ImVec4(0, 0, 0, 0);

	if (isFolder)
	{
		ImGui::PushID(id);
		if (ImGui::ImageButton((ImTextureID)icons.folder->GetGpuID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0), 0 , fileColor)) 
		{
			if (strcmp(selectedItem, path) == 0)
			{
				current_folder = path;
				selectedItem[0] = '\0';
				ret = false;
			}
			else
				sprintf_s(selectedItem, path);
		}

		ImGui::Text("%s", FileSystem::GetFileName(path).c_str());
		ImGui::PopID();
	}
	else
	{
		ImGui::PushID(id);
		if (ImGui::ImageButton((ImTextureID)icons.model->GetGpuID(), ImVec2(70, 70), ImVec2(0, 1), ImVec2(1, 0), 0, fileColor))
			sprintf_s(selectedItem, 256, path);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ASSETS", &id, sizeof(int));
			ImGui::Text("%s", FileSystem::GetFile(path).c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Button("Delete"))
			{
				App->resources->AddAssetToDelete(path);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		std::string file_name = FileSystem::GetFile(path);
		if (file_name.size() > 14)
		{
			file_name.resize(14);
			file_name.replace(file_name.end() - 3, file_name.end(), "...");
		}

		if (file_name.find(".fbx") != std::string::npos)
		{
			ImGui::SameLine();
			if (ImGui::Button("->"))
				ImGui::OpenPopup("Meshes");
			if (ImGui::BeginPopup("Meshes", ImGuiWindowFlags_NoMove))
			{
				std::string model = current_folder + "/" + file_name;
				const char* library_path = App->resources->Find(App->resources->GetUIDFromMeta(model.append(".meta").c_str()));

				std::vector<uint> meshes;
				std::vector<uint> materials;
				ModelImporter::ExtractInternalResources(library_path, meshes, materials);

				for (size_t m = 0; m < meshes.size(); m++)
				{
					ImGui::PushID(meshes[m]);
					ResourceData meshData = App->resources->RequestResourceData(meshes[m]);
					ImGui::Text("%s", meshData.name.c_str());
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						ImGui::SetDragDropPayload("MESHES", &(meshes[m]), sizeof(int));
						ImGui::EndDragDropSource();
					}
					ImGui::PopID();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::Text("%s", file_name.c_str());
		ImGui::PopID();
	}
	return ret;
}

void WindowAssets::DrawPathButtons()
{
	std::vector<std::string> splits;
	const char* path = selectedItem;

	if (selectedItem[0] == '\0')
		path = current_folder.c_str();

	FileSystem::SplitFilePath(path, &splits);
	for (size_t i = 0; i < splits.size(); i++)
	{
		if(i < splits.size()- 1)
		{
			if (ImGui::Button(splits[i].append("/").c_str(), ImVec2(splits[i].size() * ImGui::GetFontSize() * 0.75f, 20))) {
				for (size_t j = 1; j <= i; j++)
				{
					splits[0].append(splits[j]);
				}
				splits[0].erase(splits[0].end()-1);
				current_folder = splits[0].c_str();
				selectedItem[0] = '\0';
				break;
			}
		}
		else
		{
			if (ImGui::Button(splits[i].c_str(), ImVec2(splits[i].size() * ImGui::GetFontSize() * 0.75f, 20))) {
				for (size_t j = 1; j <= i; j++)
				{
					splits[0].append(splits[j]);
				}
				break;
			}
		}

		ImGui::SameLine();
	}

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
