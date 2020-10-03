#include "Application.h"
#include "Editor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "MathGeoLib/include/MathGeoLib.h"


Editor::Editor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//*open_dockspace = true;
	show_scene_window = true;
	show_inspector_window = true;
	show_project_window = true;
	show_console_window = false;
	show_configuration_window = true;

	show_preferences_window = false;

	current_theme = 1;

	fps_log.resize(100,0);
	ms_log.resize(100,0);
}

Editor::~Editor() {

}

bool Editor::Init() 
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

	//scene window
	if (show_scene_window)
	{
		ImGui::Begin("Scene", &show_scene_window);
		ImGui::End();
	}

	//inspector window
	if(show_inspector_window)
	{
		ImGui::Begin("Inspector", &show_inspector_window);                      
		ImGui::End();
	}

	//project window
	if (show_project_window)
	{
		ImGui::Begin("Project", &show_project_window); 
		ImGui::End();
	}	

	//project window
	if (show_console_window)
	{
		ImGui::Begin("Console", &show_console_window);
		ImGui::End();
	}

	//preferences
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

	//configuration
	if (show_configuration_window)
	{
		ShowConfigurationWindow();
	}

	return ret;
}

update_status Editor::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

bool Editor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status Editor::ShowDockSpace(bool* p_open) {
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
	if (CreateMainMenuBar())
		ret = UPDATE_CONTINUE;
	else
		ret = UPDATE_STOP;

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
			if (ImGui::MenuItem("Save"))
			{

			}
			else if (ImGui::MenuItem("Exit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo   Ctrl+Z")) {}
			else if (ImGui::MenuItem("Redo   Ctrl+Y")) 
			{}
			else if (ImGui::MenuItem("Configuration"))
			{
				show_configuration_window = true;
			}
			else if (ImGui::MenuItem("Preferences"))
			{
				show_preferences_window = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Inspector"))
			{
				show_inspector_window = true;
			}
			else if (ImGui::MenuItem("Scene"))
			{
				show_scene_window = true;
			}
			else if (ImGui::MenuItem("Project"))
			{
				show_project_window = true;
			}
			else if (ImGui::MenuItem("Console"))
			{
				show_console_window = true;
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

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::End();

	return ret;
}

void Editor::ShowConfigurationWindow() 
{
	ImGui::Begin("Configuration", &show_configuration_window);

	if (ImGui::CollapsingHeader("Application")) 
	{
		static int fps_cap = App->GetFPSCap();
		if (ImGui::SliderInt("Max FPS", &fps_cap, 10, 120)) {
			App->SetFPSCap(fps_cap);
		}

		char title[25];

		//FPS graph
		fps_log.erase(fps_log.begin());
		fps_log.push_back(App->GetFPS());
		//if (fps_log[fps_log.size() - 1] != 0) {
		sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		//}

		//Ms graph
		ms_log.erase(ms_log.begin());
		ms_log.push_back(App->GetLastDt());
		//if(ms_log[ms_log.size() - 1] != 0){
		sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		//}
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		static bool fullscreen = false;
		static bool fullscreen_desktop = false;
		static bool resizable = true;
		static bool borderless = false;
	

		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			App->window->SetFullscreen(fullscreen);
		
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
			App->window->SetResizable(resizable);

		if (ImGui::Checkbox("Borderless", &borderless))
			App->window->SetBorderless(borderless);
		
		ImGui::SameLine();
		if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
			App->window->SetFullscreenDesktop(fullscreen_desktop);		
		

		int width, height;
		App->window->GetSize(width, height);

		if ((ImGui::SliderInt("Width", &width, 640, 3840) || ImGui::SliderInt("Height", &height, 360, 2160)))
			App->window->SetSize(width, height);
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		SDL_version version;
		SDL_GetVersion(&version);
		ImGui::Text("SDL Version: %d.%d.%d", version.major, version.minor, version.patch);
	}

	ImGui::End();
}