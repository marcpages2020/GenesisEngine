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

	//CONSOLE_WINDOW,
}

Editor::~Editor() 
{
}

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
		LoadFile(nullptr, "Library/");

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
				App->Save("Library/Scenes/new_scene.scene");
			}
			else if (ImGui::MenuItem("Load Scene"))
			{
				file_dialog = opened;
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
				App->scene->AddGameObject(new GameObject(new GnCube()));
			}
			else if (ImGui::MenuItem("FBX: Cube"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/cube.fbx"));
			}
			else if (ImGui::MenuItem("Cylinder"))
			{
				App->scene->AddGameObject(new GameObject(new GnCylinder()));
			}
			else if (ImGui::MenuItem("FBX: Cylinder"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/cylinder.fbx"));
			}
			else if (ImGui::MenuItem("Sphere"))
			{
				App->scene->AddGameObject(new GameObject(new GnSphere()));
			}
			else if (ImGui::MenuItem("FBX: Sphere"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/sphere.fbx"));
			}
			else if (ImGui::MenuItem("Pyramid"))
			{
				App->scene->AddGameObject(new GameObject(new GnPyramid()));
			}
			else if (ImGui::MenuItem("FBX: Pyramid"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/pyramid.fbx"));
			}
			else if (ImGui::MenuItem("Plane"))
			{
				App->scene->AddGameObject(new GameObject(new GnPlane()));
			}
			else if (ImGui::MenuItem("FBX: Plane"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/plane.fbx"));
			}
			else if (ImGui::MenuItem("Cone"))
			{
				App->scene->AddGameObject(new GameObject(new GnCone()));
			}
			else if (ImGui::MenuItem("FBX: Cone"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/cone.fbx"));
			}
			else if (ImGui::MenuItem("Suzanne"))
			{
				//App->scene->AddGameObject(MeshImporter::ImportFBX("Assets/Models/Primitives/monkey.fbx"));
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
			else if (ImGui::MenuItem("Project", NULL, show_project_window))
			{
				show_project_window = !show_project_window;
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
	//ImGuiWindowFlags_NoTitleBar;
	//ImGuiWindowFlags_NoDecoration
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

	float x_position = App->window->width * 0.5f;
	float y_position = App->window->height * 0.1f;

	//ImGui::SetNextWindowPos(ImVec2(x_position - 20, y_position));
	if (ImGui::Begin("Game Buttons", &show_game_buttons, flags))
	{
		ImGui::Columns(3);

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
		if (ImGui::Button("Ok", ImVec2(50, 20)))
			file_dialog = ready_to_close;
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 20)))
		{
			file_dialog = ready_to_close;
			selected_file[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void Editor::DrawDirectoryRecursive(const char* directory, const char* filter_extension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	FileSystem::DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str()))
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

		if (ok && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
		{
			if (ImGui::IsItemClicked()) {
				sprintf_s(selected_file, 256, "%s%s", dir.c_str(), str.c_str());

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

	App->renderer3D->OnResize(image_size.x, image_size.y);
	App->camera->OnResize(image_size.x, image_size.y);
}

