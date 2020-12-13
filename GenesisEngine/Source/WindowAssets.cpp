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
		if (ImGui::TreeNodeEx("Assets", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::IsItemClicked())
				current_folder = "Assets";

			DrawDirectoryRecursive("Assets", nullptr);
			ImGui::TreePop();
		}
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
			if(open)
				ImGui::TreePop();
			break;
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
			if (App->resources->GetTypeFromPath(tmp_files[i].c_str()) != ResourceType::RESOURCE_UNKNOWN)
				files.push_back(tmp_files[i]);
		}
	}

	tmp_files.clear();

	if (previews.empty())
		LoadPreviews(files);

	int total_icons = files.size() + dirs.size();
	int icons_drawn = 0;

	int columns = floor(ImGui::GetContentRegionAvailWidth() / 105.0f);
	int rows = ceil(files.size() + dirs.size() / (float)columns);

	int dirs_drawn = 0;
	int files_drawn = 0;

	bool ret = true;

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
				UnloadPreviews();
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
		std::string meta_file = App->resources->GenerateMetaFile(path);
		if (!FileSystem::Exists(meta_file.c_str()))
			return ret;

		uint UID = App->resources->GetUIDFromMeta(meta_file.c_str());
		ImGui::PushID(UID);
		std::string file_name = FileSystem::GetFile(path);

		ResourceType type = App->resources->GetTypeFromPath(path);
		uint imageID = icons.model->GetGpuID();

		if(type == ResourceType::RESOURCE_TEXTURE)
		{
			ResourceTexture* preview = nullptr;
			
			for (std::map<std::string, ResourceTexture*>::iterator it = previews.begin(); it != previews.end(); it++)
			{
				if (it->first == file_name) 
					preview = it->second;
			}

			if (preview != nullptr)
				imageID = preview->GetGpuID();
		}

		if (ImGui::ImageButton((ImTextureID)imageID, ImVec2(75, 70), ImVec2(0, 1), ImVec2(1, 0), 0, fileColor))
			sprintf_s(selectedItem, 256, path);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ASSETS", &UID, sizeof(int));
			ImGui::Text("%s", FileSystem::GetFile(path).c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Button("Delete"))
			{
				App->resources->AddAssetToDelete(path);
				ImGui::CloseCurrentPopup();
				selectedItem[0] = '\0';
				UnloadPreviews();
				ret = false;
			}
			ImGui::EndPopup();
		}

		if (FileSystem::ToLower(file_name.c_str()).find(".fbx") != std::string::npos)
		{
			ImGui::SameLine();
			if (ImGui::Button("->", ImVec2(20,20)))
				ImGui::OpenPopup("Meshes");

			DrawModelInternalResources(file_name.c_str());
		}
		
		if (file_name.size() > 14)
		{
			file_name.resize(14);
			file_name.replace(file_name.end() - 3, file_name.end(), "...");
		}
		ImGui::Text("%s", file_name.c_str());

		ImGui::PopID();
	}
	return ret;
}

void WindowAssets::DrawModelInternalResources(const char* file_name)
{
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

void WindowAssets::DrawPathButtons()
{
	std::vector<std::string> splits;
	const char* path = selectedItem;

	if (selectedItem[0] == '\0')
		path = current_folder.c_str();

	FileSystem::SplitFilePath(path, &splits);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,1));

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
				UnloadPreviews();
				selectedItem[0] = '\0';
				break;
			}
		}
		else
		{
			ImGui::Button(splits[i].c_str(), ImVec2(splits[i].size() * ImGui::GetFontSize() * 0.75f, 20)); 
		}
		ImGui::SameLine();
	}
	ImGui::PopStyleColor(1);
}

void WindowAssets::LoadPreviews(std::vector<std::string> current_folder_files)
{
	for (size_t i = 0; i < current_folder_files.size(); i++)
	{
		if (App->resources->GetTypeFromPath(current_folder_files[i].c_str()) == ResourceType::RESOURCE_TEXTURE)
		{
			std::string path = current_folder;
			path.append("/" + current_folder_files[i]);
			ResourceTexture* preview = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(App->resources->Find(path.c_str())));
			previews[FileSystem::GetFile(current_folder_files[i].c_str())] = preview;
		}
	}
}

void WindowAssets::UnloadPreviews()
{
	for (std::map<std::string, ResourceTexture*>::iterator preview = previews.begin(); preview != previews.end(); preview++)
	{
		App->resources->ReleaseResource(preview->second->GetUID());
	}

	previews.clear();
}
