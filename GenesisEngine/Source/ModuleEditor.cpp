#include "Globals.h"
#include "Engine.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "EditorWindow.h"
#include "EditorWindow_Scene.h"
#include "EditorWindow_Assets.h"
#include "EditorWindow_Console.h"
#include "EditorWindow_Inspector.h"
#include "EditorWindow_Configuration.h"

#include "ModuleEditor.h"

ModuleEditor::ModuleEditor(GnEngine* app, bool start_enabled) : Module(app, start_enabled), isDockspaceOpen(true), showImGuiDemo(false)
{
	name = "editor";
}

ModuleEditor::~ModuleEditor() 
{}

bool ModuleEditor::Init() 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(Engine->window->window, Engine->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	windows.push_back(new EditorWindow_Scene(this));
	windows.push_back(new EditorWindow_Assets(this));
	windows.push_back(new EditorWindow_Console(this));
	windows.push_back(new EditorWindow_Inspector(this));
	windows.push_back(new EditorWindow_Configuration(this));

	return true;
}

update_status ModuleEditor::Update(float deltaTime) 
{
	update_status ret = UPDATE_CONTINUE;
	return ret;
}

update_status ModuleEditor::PostUpdate(float deltaTime)
{
	update_status ret = UPDATE_CONTINUE;

	//Update the frames
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Engine->window->window);
	ImGui::NewFrame();

	ret = ShowDockSpace(&isDockspaceOpen);

	for (size_t i = 0; i < windows.size(); ++i)
	{
		if (windows[i]->IsOpen())
		{
			windows[i]->Draw();
		}
	}

	//preferences
	/*
	if (show_preferences_window)
	{
		ImGui::Begin("Preferences", &show_preferences_window);

		//Style
		{
			// Using the _simplified_ one-liner Combo() api here
			// See "Combo" section for examples of how to use the more complete BeginCombo()/EndCombo() api.
			const char* items[] = { "Classic", "Dark", "Light"};
			if (ImGui::Combo("Interface Style", &current_theme, items, IM_ARRAYSIZE(items)))
			{
				ChangeTheme(std::string(items[current_theme]));
			}
			ImGui::SameLine();
		}

		ImGui::End();
	}
	*/

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return ret;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status ModuleEditor::ShowDockSpace(bool* p_open) {
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
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

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	// and handle the pass-thru hole, so we ask Begin() to not render a background.
	
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", p_open, window_flags);
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
	else
	{
		//ShowDockingDisabledMessage();
	}
	
	//main bar
	if (ImGui::BeginMainMenuBar())
	{
		//FILE
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				ret = UPDATE_STOP;
			}
			ImGui::EndMenu();
		}

		//EDIT
		if (ImGui::BeginMenu("Edit"))
		{
			/*
			if (ImGui::MenuItem("Undo   Ctrl+Z")) {	}
			else if (ImGui::MenuItem("Redo   Ctrl+Y")) { }
			else if (ImGui::MenuItem("Preferences"))
			{
				show_preferences_window = true;
			}
			*/
			ImGui::EndMenu();
		}

		//WINDOWS
		if (ImGui::BeginMenu("Window"))
		{
			for (size_t i = 0; i < windows.size(); ++i)
			{
				if (ImGui::MenuItem(windows[i]->GetName(), NULL, windows[i]->IsOpen()))
				{
					windows[i]->SetOpen(true);
				}
			}
			ImGui::EndMenu();
		}

		//HELP
		if (ImGui::BeginMenu("Help"))
		{
			if(ImGui::MenuItem("ImGui Demo", NULL, &showImGuiDemo)){
				ImGui::ShowDemoWindow(&showImGuiDemo);
			}

			if (ImGui::MenuItem("Download latest version")) {
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine/releases", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Report a bug")) {
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine/issues", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("View on GitHub")) {
				ShellExecuteA(NULL, "open", "https://github.com/marcpages2020/GenesisEngine", NULL, NULL, SW_SHOWNORMAL);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::End();

	return ret;
}

void ModuleEditor::ChangeTheme(std::string theme) 
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