#include "Engine.h"
#include "ModuleEditor.h"
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

#include "glad/include/glad/glad.h"
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
#include "WindowShaderEditor.h"
#include "WindowConsole.h"

#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled), 
open_dockspace(true), show_preferences_window(false), show_project_window(false),
currentTheme(EditorTheme::DARK)
{
	name = "editor";

	show_game_buttons = true;

	image_size = { 0,0 };

	//CONSOLE_WINDOW,
	scene_name[0] = '\0';
	selected_file[0] = '\0';
	selected_folder[0] = '\0';

	AddWindow(new WindowAbout());
	AddWindow(new WindowAssets());
	AddWindow(new WindowConfiguration());
	AddWindow(new WindowConsole());
	AddWindow(new WindowHierarchy());
	AddWindow(new WindowImport());
	AddWindow(new WindowInspector());
	AddWindow(new WindowScene());
	AddWindow(new WindowShaderEditor());
}

ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Init()
{
	return true;
}

bool ModuleEditor::Start()
{
	bool ret = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(engine->window->window, engine->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	for (std::map<const char*, EditorWindow*>::iterator window = windows.begin(); window != windows.end(); ++window)
	{
		if (window->second->visible)
		{
			window->second->Init();
		}
	}

	return ret;
}

update_status ModuleEditor::Update(float deltaTime)
{
	update_status ret = UPDATE_CONTINUE;

	return ret;
}

update_status ModuleEditor::Draw()
{
	//Update the frames
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(engine->window->window);
	ImGui::NewFrame();

	ShowDockSpace(&open_dockspace);

	for (std::map<const char*, EditorWindow*>::iterator window = windows.begin(); window != windows.end(); ++window)
	{
		if (window->second->visible)
		{
			window->second->Draw();
		}
	}

	ShowGameButtons();

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

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (std::map<const char*, EditorWindow*>::iterator window = windows.begin(); window != windows.end(); ++window)
	{
		if (window->second->visible)
		{
			delete window->second;
			window->second = nullptr;
		}
	}

	windows.clear();

	return true;
}

bool ModuleEditor::LoadConfig(GnJSONObj& config)
{
	GnJSONArray jsonWindows(config.GetArray("windows"));

	GnJSONObj jsonWindow;
	for (std::map<const char*, EditorWindow*>::iterator window = windows.begin(); window != windows.end(); ++window)
	{
		//Lowercase the string
		std::string windowName = std::string(window->second->GetName());
		std::transform(windowName.begin(), windowName.end(), windowName.begin(), [](unsigned char c) { return std::tolower(c); });

		jsonWindow = jsonWindows.GetObjectInArray(windowName.c_str());
		if (jsonWindow.GetValue())
		{
			window->second->visible = jsonWindow.GetBool("visible");
		}
	}

	return true;
}

bool ModuleEditor::IsWindowFocused(const char* windowName)
{
	EditorWindow* windowToCheck = GetWindow(windowName);

	if (windowToCheck) { return windowToCheck->visible; }
	else return false;
}

bool ModuleEditor::MouseOnScene()
{
	return mouseScenePosition.x > 0 && mouseScenePosition.x < image_size.x
		&& mouseScenePosition.y > 0 && mouseScenePosition.y < image_size.y;
}

EditorWindow* ModuleEditor::GetWindow(const char* windowName)
{
	return windows[windowName];
}

update_status ModuleEditor::ShowDockSpace(bool* p_open)
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

	if (ImGui::Begin("DockSpace", p_open, window_flags)) {
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

void ModuleEditor::ChangeTheme(EditorTheme newTheme)
{
	currentTheme = newTheme;

	switch (currentTheme)
	{
	case EditorTheme::LIGHT:
		ImGui::StyleColorsLight();
		LOG("Theme changed to: Light");
		break;
	case EditorTheme::CLASSIC:
		ImGui::StyleColorsClassic();
		LOG("Theme changed to: Classic");
		break;
	case EditorTheme::DARK:
		ImGui::StyleColorsDark();
		LOG("Theme changed to: Dark");
		break;
	default:
		break;
	}
}

void ModuleEditor::AddWindow(EditorWindow* windowToAdd)
{
	if (!windowToAdd) { return; }
	windows[windowToAdd->GetName()] = windowToAdd;
}

bool ModuleEditor::CreateMainMenuBar() {
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
				WindowConfiguration* configurationWindow = (WindowConfiguration*)GetWindow("Configuration");
				if (configurationWindow)
				{
					configurationWindow->visible = true;
				}
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
				engine->scene->AddGameObject(new GameObject());
			}
			else if (ImGui::MenuItem("Cube"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/cube.fbx"));
			}
			else if (ImGui::MenuItem("Cylinder"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/cylinder.fbx"));
			}
			else if (ImGui::MenuItem("Sphere"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/sphere.fbx"));
			}
			else if (ImGui::MenuItem("Pyramid"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/pyramid.fbx"));
			}
			else if (ImGui::MenuItem("Plane"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/plane.fbx"));
			}
			else if (ImGui::MenuItem("Cone"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/cone.fbx"));
			}
			else if (ImGui::MenuItem("Torus"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/torus.fbx"));
			}
			else if (ImGui::MenuItem("Suzanne"))
			{
				engine->scene->AddGameObject(engine->resources->RequestGameObject("Assets/EngineAssets/Primitives/monkey.fbx"));
			}
			else if (ImGui::MenuItem("Camera"))
			{
				engine->scene->AddGameObject(new GameObject(ComponentType::CAMERA));
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			for (std::map<const char*, EditorWindow*>::iterator window = windows.begin(); window != windows.end(); ++window)
			{
				if (ImGui::MenuItem(window->second->GetName(), NULL, window->second->visible))
				{
					window->second->visible = !window->second->visible;
				}
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

			WindowAbout* windowAbout = (WindowAbout*)GetWindow("About");
			if (windowAbout && ImGui::MenuItem("About"))
			{
				windowAbout->visible = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return ret;
}

void ModuleEditor::ShowGameButtons()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration;

	ImGui::SetNextWindowSize(ImVec2(130, 40));
	if (ImGui::Begin("Game Buttons", &show_game_buttons, flags))
	{
		ImGui::Columns(3);

		//ImGui::RadioButton("Global", true);
		ImGui::SameLine();
		//ImGui::RadioButton("Local", true);

		ImGui::NextColumn();
		if (engine->in_game == false)
		{
			if (ImGui::Button("Play", ImVec2(40, 20)))
				engine->StartGame();
		}
		else {
			if (ImGui::Button("Stop", ImVec2(40, 20)))
				engine->StopGame();
		}

		ImGui::SameLine();

		//ImGui::NextColumn();
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
		ImGui::NextColumn();

	}
	ImGui::End();
}

void ModuleEditor::LoadFile(const char* filter_extension, const char* from_dir)
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
			engine->Load(selected_file);
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

void ModuleEditor::SaveFile(const char* filter_extension, const char* from_dir)
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
			engine->Save(selected_file);

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
			engine->Save(selected_file);
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

void ModuleEditor::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
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

			if (ImGui::IsItemClicked())
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
		if (strcmp(selected_file, complete_path.c_str()) == 0)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (ok && ImGui::TreeNodeEx(str.c_str(), flags))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selected_file, 256, "%s/%s", dir.c_str(), str.c_str());

				if (ImGui::IsMouseDoubleClicked(0))
				{
					file_dialog = ready_to_close;
					if (scene_operation == SceneOperation::SAVE)
					{
						engine->Save(selected_file);
					}
					else if (scene_operation == SceneOperation::LOAD)
					{
						engine->Load(selected_file);
					}
				}
			}

			ImGui::TreePop();
		}
	}
}

void ModuleEditor::ShowPreferencesWindow()
{
	if (ImGui::Begin("Preferences", &show_preferences_window)) {
		//Style
		const char* items[] = { "Light", "Classic", "Dark" };
		int themeIndex = (int)currentTheme;
		if (ImGui::Combo("Interface Style", &themeIndex, items, IM_ARRAYSIZE(items)))
		{
			ChangeTheme((EditorTheme)themeIndex);
		}
	}
	ImGui::End();
}

void ModuleEditor::OnResize(ImVec2 window_size)
{
	image_size = window_size;

	engine->camera->OnResize(image_size.x, image_size.y);
	engine->renderer3D->OnResize(image_size.x, image_size.y);
}

