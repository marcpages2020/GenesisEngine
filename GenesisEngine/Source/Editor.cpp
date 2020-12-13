#include "Application.h"
#include "Editor.h"
#include "GnJSON.h"
#include "Mesh.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "FileSystem.h"
#include "Camera.h"
#include "Time.h"

#include <vector>
#include <string>
#include <algorithm>

#include "glew/include/glew.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "MathGeoLib/include/MathGeoLib.h"

//Windows
#include "WindowHierarchy.h"
#include "WindowInspector.h"
#include "WindowScene.h"
#include "WindowAssets.h"
#include "WindowConfiguration.h"
#include "WindowAbout.h"
#include "WindowImport.h"

#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

Editor::Editor(bool start_enabled) : Module(start_enabled)
{
	name = "editor";

	scene_window_focused = false;
	show_game_buttons = true;

	current_theme = 1;

	image_size = { 0,0 };

	windows[HIERARCHY_WINDOW] = new WindowHierarchy();
	windows[INSPECTOR_WINDOW] = new WindowInspector();
	windows[SCENE_WINDOW] = new WindowScene();
	windows[ASSETS_WINDOW] = new WindowAssets();
	windows[CONFIGURATION_WINDOW] = new WindowConfiguration();
	windows[ABOUT_WINDOW] = new WindowAbout();
	windows[IMPORT_WINDOW] = new WindowImport();

	//CONSOLE_WINDOW,
	scene_name[0] = '\0';
	selected_file[0] = '\0';
	selected_folder[0] = '\0';
}

Editor::~Editor() {}

bool Editor::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

bool Editor::Start()
{
	bool ret = true;

	for (size_t i = 0; i < MAX_WINDOWS; i++)
	{
		windows[i]->Init();
	}

	return ret;
}

update_status Editor::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	//Update the frames
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ret = ShowDockSpace(open_dockspace);

	return ret;
}

update_status Editor::Draw()
{
	for (size_t i = 0; i < MAX_WINDOWS; i++)
	{
		if(windows[i]->visible)
			windows[i]->Draw();
	}

	ShowGameButtons();

	//Console
	if (show_console_window)
	{
		if (ImGui::Begin("Console", &show_console_window, ImGuiWindowFlags_MenuBar)) {

			if (ImGui::BeginMenuBar()) 
			{
				if (ImGui::MenuItem("Clear"))
				{
					console_log.clear();
				}
				ImGui::EndMenuBar();
			}

			for (int i = 0; i < console_log.size(); i++)
			{
				if (console_log[i].warning_level == 0) //Normal log
				{
					ImGui::Text(console_log[i].log_text.c_str());
				}
				else if (console_log[i].warning_level == 1) //Warning
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), console_log[i].log_text.c_str());
				}
				else //Error
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), console_log[i].log_text.c_str());
			}
		}
		ImGui::End();
	}

	//Preferences
	if (show_preferences_window)
		ShowPreferencesWindow();

	if (file_dialog == opened)
	{
		if (scene_operation == SceneOperation::LOAD)
			LoadFile(".scene", "Library/");
		else if (scene_operation == SceneOperation::SAVE) {
			sprintf_s(selected_folder, 256, "Library/Scenes");
			SaveFile(".scene", "Library/");
		}
	}

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool Editor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	console_log.clear();

	for (size_t i = 0; i < MAX_WINDOWS; i++)
	{
		delete windows[i];
		windows[i] = nullptr;
	}

	return true;
}

bool Editor::LoadConfig(GnJSONObj& config)
{
	GnJSONArray jsonWindows(config.GetArray("windows"));
	
	GnJSONObj window = jsonWindows.GetObjectInArray("scene");
	windows[SCENE_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("inspector");
	windows[INSPECTOR_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("hierarchy");
	windows[HIERARCHY_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("assets");
	windows[ASSETS_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("console");
	show_console_window = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("configuration");
	windows[CONFIGURATION_WINDOW]->visible = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("preferences");
	show_preferences_window = window.GetBool("visible");

	window = jsonWindows.GetObjectInArray("about");
	windows[ABOUT_WINDOW]->visible = window.GetBool("visible");
	
	return true;
}

bool Editor::IsSceneFocused()
{
	return scene_window_focused;
}

bool Editor::MouseOnScene()
{
	return mouseScenePosition.x > 0 && mouseScenePosition.x < image_size.x 
		   && mouseScenePosition.y > 0 && mouseScenePosition.y < image_size.y;
}

void Editor::AddConsoleLog(const char* log, int warning_level)
{
	log_message message = { log, warning_level };
	console_log.push_back(message);
}

update_status Editor::ShowDockSpace(bool* p_open) 
{
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	if(ImGui::Begin("DockSpace", p_open, window_flags)){
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		//main bar
		if (CreateMainMenuBar())
			ret = UPDATE_CONTINUE;
		else
			ret = UPDATE_STOP;
	}
	ImGui::End();

	return ret;
}

void Editor::ChangeTheme(std::string theme)
{
	if (theme == "Dark")
	{
		ImGui::StyleColorsDark();
	}
	else if (theme == "Classic")
	{
		ImGui::StyleColorsClassic();
	}
	else if (theme == "Light")
	{
		ImGui::StyleColorsLight();
	}
}

bool Editor::CreateMainMenuBar() {
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene"))
			{
				file_dialog = opened;
				scene_operation = SceneOperation::SAVE;
			}
			else if (ImGui::MenuItem("Load Scene"))
			{
				file_dialog = opened;
				scene_operation = SceneOperation::LOAD;
			}
			else if (ImGui::MenuItem("Exit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Configuration"))
			{
				windows[CONFIGURATION_WINDOW]->visible = true;
			}
			else if (ImGui::MenuItem("Preferences"))
			{
				show_preferences_window = true;
			}
			ImGui::EndMenu();
		}
	
		if (ImGui::BeginMenu("Game Object"))
		{
			if (ImGui::MenuItem("Empty Object"))
			{
				App->scene->AddGameObject(new GameObject());
			}
			else if (ImGui::MenuItem("Cube"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/cube.fbx"));
			}
			else if (ImGui::MenuItem("Cylinder"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/cylinder.fbx"));
			}
			else if (ImGui::MenuItem("Sphere"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/sphere.fbx"));
			}
			else if (ImGui::MenuItem("Pyramid"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/pyramid.fbx"));
			}
			else if (ImGui::MenuItem("Plane"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/plane.fbx"));
			}
			else if (ImGui::MenuItem("Cone"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/cone.fbx"));
			}
			else if (ImGui::MenuItem("Torus"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/torus.fbx"));
			}
			else if (ImGui::MenuItem("Suzanne"))
			{
				App->scene->AddGameObject(App->resources->RequestGameObject("Assets/EngineAssets/Primitives/monkey.fbx"));
			}
			else if (ImGui::MenuItem("Camera"))
			{
				App->scene->AddGameObject(new GameObject(ComponentType::CAMERA));
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Inspector", NULL, windows[INSPECTOR_WINDOW]->visible))
			{
				windows[INSPECTOR_WINDOW]->visible = !windows[INSPECTOR_WINDOW]->visible;
			}
			else if (ImGui::MenuItem("Hierarchy", NULL, windows[HIERARCHY_WINDOW]->visible))
			{
				windows[HIERARCHY_WINDOW]->visible = !windows[HIERARCHY_WINDOW]->visible;
			}
			else if (ImGui::MenuItem("Scene", NULL, windows[SCENE_WINDOW]->visible))
			{
				windows[SCENE_WINDOW]->visible = !windows[SCENE_WINDOW]->visible;
			}
			else if (ImGui::MenuItem("Assets", NULL, windows[ASSETS_WINDOW]->visible))
			{
				windows[ASSETS_WINDOW]->visible = !windows[ASSETS_WINDOW]->visible;
			}
			else if (ImGui::MenuItem("Console", NULL, show_console_window))
			{
				show_console_window = !show_console_window;
			}
			else if (ImGui::MenuItem("Configuration", NULL, windows[CONFIGURATION_WINDOW]->visible))
			{
				windows[CONFIGURATION_WINDOW]->visible = !windows[CONFIGURATION_WINDOW]->visible;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine/wiki", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Download latest"))
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine/releases", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("Report a bug"))
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine/issues", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("View on GitHub"))
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine", NULL, NULL, SW_SHOWNORMAL);

			if (ImGui::MenuItem("About"))
				windows[ABOUT_WINDOW]->visible = true;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return ret;
}

void Editor::ShowGameButtons()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowSize(ImVec2(130, 40));
	if (ImGui::Begin("Game Buttons", &show_game_buttons, flags))
	{
		ImGui::Columns(2);

		if (App->in_game == false)
		{
			if (ImGui::Button("Play", ImVec2(40, 20)))
				App->StartGame();
		}
		else {
			if (ImGui::Button("Stop", ImVec2(40, 20)))
				App->StopGame();
		}

		ImGui::NextColumn();
		if (Time::gameClock.paused) 
		{
			if (ImGui::Button("Resume", ImVec2(45, 20)))
				Time::gameClock.Resume();
		}
		else 
		{
			if (ImGui::Button("Pause", ImVec2(45, 20))) 
				Time::gameClock.Pause();
		}

	}
	ImGui::End();
}

void Editor::LoadFile(const char* filter_extension, const char* from_dir)
{
	ImGui::OpenPopup("Load File");
	if (ImGui::BeginPopupModal("Load File", nullptr))
	{
		in_modal = true;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		 DrawDirectoryRecursive(from_dir, filter_extension);
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		if (ImGui::InputText("##file_selector", selected_file, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			file_dialog = ready_to_close;

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Ok", ImVec2(50, 20))) {
			file_dialog = ready_to_close;
			App->Load(selected_file);
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
			file_dialog = ready_to_close;

		if (file_dialog == ready_to_close)
		{
			selected_file[0] = '\0';
			strcpy(selected_folder, "Library/Scenes");
		}

		ImGui::EndPopup();
	}
}

void Editor::SaveFile(const char* filter_extension, const char* from_dir)
{
	ImGui::OpenPopup("Save File");
	if (ImGui::BeginPopupModal("Save File", nullptr))
	{
		in_modal = true;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		DrawDirectoryRecursive(from_dir, filter_extension);
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::PushItemWidth(250.f);
		if (ImGui::InputText("##file_selector", scene_name, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			file_dialog = ready_to_close;
			if (scene_name[0] == '\0')
				strcpy(scene_name, "untitled");

			sprintf_s(selected_file, 128, "%s/%s.scene", selected_folder, scene_name);
			App->Save(selected_file);

			selected_file[0] = '\0';
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Ok", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			if (scene_name[0] == '\0')
				strcpy(scene_name, "untitled");

			sprintf_s(selected_file, 128, "%s/%s.scene", selected_folder, scene_name);
			App->Save(selected_file);
		}
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
		}

		if (file_dialog == ready_to_close)
		{
			selected_file[0] = '\0';
			strcpy(selected_folder, "Library/Scenes");
			scene_name[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void Editor::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

	std::string dir((directory) ? directory : "");

	FileSystem::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		std::string folder = (dir + (*it)).c_str();
		if (strcmp(folder.c_str(), selected_folder) == 0)
			flags = ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(folder.c_str(), flags, "%s/", (*it).c_str()))
		{
			flags = ImGuiTreeNodeFlags_None;

			if(ImGui::IsItemClicked())
				sprintf_s(selected_folder, 256, "%s%s", directory, (*it).c_str());

			DrawDirectoryRecursive((dir + (*it)).c_str(), filter_extension);
			ImGui::TreePop();
		}

		flags = ImGuiTreeNodeFlags_None;
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool ok = true;

		if (filter_extension && str.find(filter_extension) == std::string::npos)
			ok = false;

		flags = ImGuiTreeNodeFlags_Leaf;

		std::string complete_path = std::string(directory) + "/" + str;
		if(strcmp(selected_file, complete_path.c_str()) == 0)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (ok && ImGui::TreeNodeEx(str.c_str(), flags))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selected_file, 256, "%s/%s", dir.c_str(), str.c_str());

				if (ImGui::IsMouseDoubleClicked(0))
				{
					file_dialog = ready_to_close;
					App->Load(selected_file);
				}
			}

			ImGui::TreePop();
		}
	}
}

void Editor::OnResize(ImVec2 window_size)
{
	image_size = window_size;

	App->camera->OnResize(image_size.x, image_size.y);
	App->renderer3D->OnResize(image_size.x, image_size.y);
}

