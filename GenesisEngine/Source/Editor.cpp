#include "Application.h"
#include "Editor.h"

#include "glew/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "MathGeoLib/include/MathGeoLib.h"


Editor::Editor(bool start_enabled) : Module(start_enabled)
{
	name = "editor";
	//*open_dockspace = true;
	show_scene_window = true;
	show_inspector_window = true;
	show_hierachy_window = true;
	show_project_window = true;
	show_console_window = true;
	show_configuration_window = false;

	show_preferences_window = false;
	show_about_window = false;

	current_theme = 1;

	fps_log.resize(100, 0);
	ms_log.resize(100, 0);
}

Editor::~Editor() {

}

bool Editor::Init(JSON_Object* object)
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
		ImGui::Begin("Scene", &show_scene_window, ImGuiWindowFlags_NoBackground);
		const char* items[] = { "Solid", "Wireframe" };
		static int current_display = App->renderer3D->GetDisplayMode();
		if (ImGui::Combo("Display Mode", &current_display, items, IM_ARRAYSIZE(items)))
		{
			App->renderer3D->SetDisplayMode((DisplayMode)current_display);
		}
		ImGui::End();
	}

	//Inspector
	if (show_inspector_window)
	{
		ImGui::Begin("Inspector", &show_inspector_window);
		ImGui::End();
	}

	//Project 
	if (show_project_window)
	{
		ImGui::Begin("Project", &show_project_window);
		ImGui::End();
	}

	//Hieracy
	if (show_hierachy_window)
	{
		ImGui::Begin("Hierachy", &show_hierachy_window);
		ImGui::End();
	}


	//Console
	if (show_console_window)
	{
		ImGui::Begin("Console", &show_console_window);
		for (int i = 0; i < console_log.size(); i++)
		{
			if (console_log[i].error) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), console_log[i].log_text.c_str());
			}
			else
				ImGui::Text(console_log[i].log_text.c_str());
		}
		ImGui::End();
	}

	//Preferences
	if (show_preferences_window)
	{
		ImGui::Begin("Preferences", &show_preferences_window);

		//Style
		{
			const char* items[] = { "Classic", "Dark", "Light" };
			if (ImGui::Combo("Interface Style", &current_theme, items, IM_ARRAYSIZE(items)))
			{
				ChangeTheme(std::string(items[current_theme]));
			}
			ImGui::SameLine();
		}

		ImGui::End();
	}

	//Configuration
	if (show_configuration_window)
	{
		ShowConfigurationWindow();
	}

	//About
	if (show_about_window)
	{
		ShowAboutWindow();
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

	fps_log.clear();
	ms_log.clear();
	console_log.clear();

	return true;
}

void Editor::AddConsoleLog(const char* log, bool error)
{
	log_message message = { log, error };
	console_log.push_back(message);
}

update_status Editor::ShowDockSpace(bool* p_open) {
	update_status ret = UPDATE_CONTINUE;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("DockSpace", p_open, window_flags);

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

void Editor::GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved)
{
	if (strcmp(gpu_brand, "NVIDIA Corporation") == 0)
	{
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available);
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);
	}
	else if (strcmp(gpu_brand, "ATI Technologies") == 0)
	{
		//glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		vram_budget = -1;
		//glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		vram_usage = -1;
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, &vram_available);
		//glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);
		vram_reserved = -1;
	}
}

bool Editor::CreateMainMenuBar() {
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Configuration"))
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

			if (ImGui::MenuItem("About"))
				show_about_window = true;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::End();

	return ret;
}

void Editor::ShowConfigurationWindow()
{
	if (ImGui::Begin("Configuration", &show_configuration_window))
	{
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
			ms_log.push_back(App->GetLastDt() * 1000);
			//if(ms_log[ms_log.size() - 1] != 0){
			sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
			//}
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			static float brightness = App->window->GetBrightness();
			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
				App->window->SetBrightness(brightness);

			static int width, height;
			App->window->GetSize(width, height);

			if ((ImGui::SliderInt("Width", &width, 640, 3840) || ImGui::SliderInt("Height", &height, 360, 2160)))
				App->window->SetSize(width, height);

			static bool fullscreen = App->window->fullscreen;
			static bool fullscreen_desktop = App->window->fullscreen_desktop;
			static bool resizable = App->window->resizable;
			static bool borderless = App->window->borderless;

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

		}


		if (ImGui::CollapsingHeader("Renderer"))
		{
			static bool depth_test = glIsEnabled(GL_DEPTH_TEST);
			static bool cull_face = glIsEnabled(GL_CULL_FACE);
			static bool lighting = glIsEnabled(GL_LIGHTING);
			static bool color_material = glIsEnabled(GL_COLOR_MATERIAL);
			static bool texture_2D = glIsEnabled(GL_TEXTURE_2D);

			if (ImGui::Checkbox("Depth Test", &depth_test))
				App->renderer3D->SetCapActive(GL_DEPTH_TEST, depth_test);

			ImGui::SameLine();
			if (ImGui::Checkbox("Cull Face", &cull_face))
				App->renderer3D->SetCapActive(GL_CULL_FACE, cull_face);

			if (ImGui::Checkbox("Texture 2D", &texture_2D))
				App->renderer3D->SetCapActive(GL_TEXTURE_2D, texture_2D);

			ImGui::SameLine();
			if (ImGui::Checkbox("Lighting", &lighting))
				App->renderer3D->SetCapActive(GL_LIGHTING, lighting);


			if (ImGui::Checkbox("Color Material", &color_material))
				App->renderer3D->SetCapActive(GL_COLOR_MATERIAL, color_material);

			//TODO:	Add two more enables
		}

		if (ImGui::CollapsingHeader("Camera")) {
			//static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static ImVec4 color = ImVec4(App->camera->background.r, App->camera->background.g, App->camera->background.b, App->camera->background.a);

			if (ImGui::ColorEdit3("Background Color##1", (float*)&color)) {
				App->camera->SetBackgroundColor(color.x, color.y, color.z, color.w);
			}
		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImVec4 values_color(1.0f, 1.0f, 0.0f, 1.0f);

			//SDL Version
			SDL_version version;
			SDL_GetVersion(&version);
			ImGui::Text("SDL Version:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%d.%d.%d", version.major, version.minor, version.patch);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//Hardware
			static HardwareSpecs specs = App->GetHardware();
			//CPU
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%d (Cache: %dkb)", specs.cpu_count, specs.cache);
			//RAM
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Gb", specs.ram);
			//Caps
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.caps.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			//GPU
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.gpu);

			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%s", specs.gpu_brand);

			//VRAM
			GLint vram_budget, vram_usage, vram_available, vram_reserved;

			GetMemoryStatistics(specs.gpu_brand, vram_budget, vram_usage, vram_available, vram_reserved);

			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_budget * 0.001f);

			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_available * 0.001f);

			/*
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_usage * 0.001f);

			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(values_color, "%.1f Mb", vram_reserved * 0.001f);
			*/

		}
	}
	ImGui::End();
}

void Editor::ShowAboutWindow()
{
	if (ImGui::Begin("About", &show_about_window))
	{
		static const char* engine_version = App->GetEngineVersion();
		ImGui::Text("%s v%s", App->engine_name, engine_version);
		ImGui::Text("The first chapter of your creation");
		ImGui::Spacing();

		ImGui::Text("Made by: ");
		ImGui::SameLine();
		if (ImGui::SmallButton("Marc Pages Francesch"))
			ShellExecuteA(NULL, "open", "https://github.com/marcpages2020", NULL, NULL, SW_SHOWNORMAL);


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("External libraries used: ");

		//SDL Version
		ImGui::BulletText("SDL %d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

		//OpenGL Version
		static GLint openGL_major = -1;
		static GLint openGL_minor = -1;

		if (openGL_major == -1)
			glGetIntegerv(GL_MAJOR_VERSION, &openGL_major);
		if (openGL_minor == -1)
			glGetIntegerv(GL_MINOR_VERSION, &openGL_minor);

		ImGui::BulletText("OpenGL %d.%d", openGL_major, openGL_minor);

		//MathGeoLib
		ImGui::BulletText("MathGeoLib 1.5");

		//ImGui
		static const char* imgui_version = { ImGui::GetVersion() };
		ImGui::BulletText("ImGui %s", imgui_version);

		//Glew
		ImGui::BulletText("Glew %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("License: ");
		ImGui::Spacing();

		ImGui::Text("MIT License");
		ImGui::Spacing();

		ImGui::TextWrapped("Copyright (c) 2020 Marc Pages Francesch");
		ImGui::Spacing();
		ImGui::TextWrapped(
			"Permission is hereby granted, free of charge, to any person obtaining a copy"
			"of this software and associated documentation files Genesis Engine, to deal"
			"in the Software without restriction, including without limitation the rights"
			"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell"
			"copies of the Software, and to permit persons to whom the Software is"
			"furnished to do so, subject to the following conditions : ");
		ImGui::Spacing();

		ImGui::TextWrapped(
			"The above copyright notice and this permission notice shall be included in all"
			"copies or substantial portions of the Software.");
		ImGui::Spacing();

		ImGui::TextWrapped(
			"THE SOFTWARE IS PROVIDED 'AS I', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE"
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"
			"SOFTWARE.");
	}
	ImGui::End();
}
