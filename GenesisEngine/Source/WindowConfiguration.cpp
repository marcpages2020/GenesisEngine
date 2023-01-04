#include "WindowConfiguration.h"
#include "ImGui/imgui.h"
#include "Engine.h"
#include "glad/include/glad/glad.h"
#include "Camera.h"
#include "Time.h"


WindowConfiguration::WindowConfiguration() : EditorWindow() 
{
	type = WindowType::WINDOW_CONFIGURATION;

	fps_log.resize(100, 0);
	ms_log.resize(100, 0);
}

WindowConfiguration::~WindowConfiguration() 
{
	fps_log.clear();
	ms_log.clear();
}

void WindowConfiguration::Draw()
{
	if (ImGui::Begin("Configuration", &visible))
	{
		focused = ImGui::IsWindowFocused();
		if (ImGui::CollapsingHeader("Engine"))
		{
			static int fps_cap = engine->GetFPSCap();
			if (ImGui::SliderInt("Max FPS", &fps_cap, 10, 120)) {
				engine->SetFPSCap(fps_cap);
			}

			char title[25];
			//FPS graph
			fps_log.erase(fps_log.begin());
			fps_log.push_back(engine->GetFPS());
			//if (fps_log[fps_log.size() - 1] != 0) {
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			//}

			//Ms graph
			ms_log.erase(ms_log.begin());
			ms_log.push_back(engine->GetLastDt() * 1000);
			//if(ms_log[ms_log.size() - 1] != 0){
			sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
			//}
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			static float brightness = engine->window->GetBrightness();
			if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
				engine->window->SetBrightness(brightness);

			static int width, height;
			engine->window->GetSize(width, height);

			if ((ImGui::SliderInt("Width", &width, 640, 3840) || ImGui::SliderInt("Height", &height, 360, 2160)))
				engine->window->SetSize(width, height);

			static bool fullscreen = engine->window->fullscreen;
			static bool fullscreen_desktop = engine->window->fullscreen_desktop;
			static bool resizable = engine->window->resizable;
			static bool borderless = engine->window->borderless;

			if (ImGui::Checkbox("Fullscreen", &fullscreen))
				engine->window->SetFullscreen(fullscreen);

			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable))
				engine->window->SetResizable(resizable);

			if (ImGui::Checkbox("Borderless", &borderless))
				engine->window->SetBorderless(borderless);

			ImGui::SameLine();
			if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
				engine->window->SetFullscreenDesktop(fullscreen_desktop);

		}

		if (ImGui::CollapsingHeader("Renderer"))
		{
			static bool depth_test = glIsEnabled(GL_DEPTH_TEST);
			static bool cull_face = glIsEnabled(GL_CULL_FACE);
			static bool lighting = glIsEnabled(GL_LIGHTING);
			static bool color_material = glIsEnabled(GL_COLOR_MATERIAL);
			static bool texture_2D = glIsEnabled(GL_TEXTURE_2D);
			static bool vsync = engine->renderer3D->vsync;

			if (ImGui::Checkbox("Depth Test", &depth_test))
				engine->renderer3D->SetCapActive(GL_DEPTH_TEST, depth_test);

			ImGui::SameLine();
			if (ImGui::Checkbox("Cull Face", &cull_face))
				engine->renderer3D->SetCapActive(GL_CULL_FACE, cull_face);

			if (ImGui::Checkbox("Texture 2D", &texture_2D))
				engine->renderer3D->SetCapActive(GL_TEXTURE_2D, texture_2D);

			ImGui::SameLine();
			if (ImGui::Checkbox("Lighting", &lighting))
				engine->renderer3D->SetCapActive(GL_LIGHTING, lighting);


			if (ImGui::Checkbox("Color Material", &color_material))
				engine->renderer3D->SetCapActive(GL_COLOR_MATERIAL, color_material);

			if (ImGui::Checkbox("VSYNC", &vsync))
				engine->renderer3D->SetVSYNC(vsync);

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Checkbox("Draw AABBs", &engine->renderer3D->draw_aabbs);

			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Camera")) {
			//static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			static ImVec4 color = ImVec4(engine->camera->background.r, engine->camera->background.g, engine->camera->background.b, engine->camera->background.a);

			if (ImGui::ColorEdit3("Background Color##1", (float*)&color)) {
				engine->camera->SetBackgroundColor(color.x, color.y, color.z, color.w);
			}

			ImGui::SliderFloat("Movement Speed", &engine->camera->move_speed, 0.0f, 50.0f);
			ImGui::SliderFloat("Drag Speed", &engine->camera->orbit_speed, 0.0f, 10.0f);
			ImGui::SliderFloat("Zoom Speed", &engine->camera->zoom_speed, 0.0f, 50.0f);
			ImGui::SliderFloat("Sensitivity", &engine->camera->sensitivity, 0.0f, 50.0f);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			bool fixedVerticalFOV = engine->camera->GetFixedFOV() == FixedFOV::FIXED_VERTICAL_FOV;
			bool fixedHorizontalFOV = engine->camera->GetFixedFOV() == FixedFOV::FIXED_HORIZONTAL_FOV;

			if (ImGui::RadioButton("Fixed VerticalFOV", fixedVerticalFOV))
				engine->camera->SetFixedFOV(FixedFOV::FIXED_VERTICAL_FOV);
			ImGui::SameLine();
			if (ImGui::RadioButton("Fixed HorizontalFOV", fixedHorizontalFOV))
				engine->camera->SetFixedFOV(FixedFOV::FIXED_HORIZONTAL_FOV);

			ImGui::Spacing();

			//Fixed Vertical FOV Settings
			if (fixedVerticalFOV)
			{
				float verticalFOV = engine->camera->GetVerticalFieldOfView() * RADTODEG;
				if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
					engine->camera->SetVerticalFieldOfView(verticalFOV * DEGTORAD, engine->editor->image_size.x, engine->editor->image_size.y);

				ImGui::Spacing();
				ImGui::Text("Horizontal FOV: %.2f", (float)engine->camera->GetHorizontalFieldOfView() * RADTODEG);
			}
			//Fixed Horizontal FOV Settings
			else
			{
				float horizontalFOV = engine->camera->GetHorizontalFieldOfView() * RADTODEG;
				if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
					engine->camera->SetHorizontalFieldOfView(horizontalFOV * DEGTORAD, engine->editor->image_size.x, engine->editor->image_size.y);

				ImGui::Spacing();
				ImGui::Text("Vertical FOV: %.2f", engine->camera->GetVerticalFieldOfView() * RADTODEG);
			}
			float3 camera_position = engine->camera->GetPosition();
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera_position.x, camera_position.y, camera_position.z);

			ImGui::Spacing();
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
			static HardwareSpecs specs = engine->GetHardware();
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

		}

		if (ImGui::CollapsingHeader("Input"))
		{
			ImGui::Text("Mouse X: %d", engine->input->GetMouseX());
			ImGui::Text("Mouse Y: %d", engine->input->GetMouseY());

			ImGui::Spacing();

			ImGui::Text("Current Window Mouse X: %.2f", engine->editor->mouseScenePosition.x);
			ImGui::Text("Current Window Mouse Y: %.2f", engine->editor->mouseScenePosition.y);

			ImGui::Spacing();

			ImGui::Text("Normalized Mouse X: %.2f", engine->editor->mouseScenePosition.x / engine->editor->image_size.x);
			ImGui::Text("Normalized Mouse Y: %.2f", engine->editor->mouseScenePosition.y / engine->editor->image_size.y);

			ImGui::Spacing();

			float normalized_x = engine->editor->mouseScenePosition.x / engine->editor->image_size.x;
			float normalized_y = engine->editor->mouseScenePosition.y / engine->editor->image_size.y;

			normalized_x = (normalized_x - 0.5f) * 2.0f;
			normalized_y = -(normalized_y - 0.5f) * 2.0f;

			ImGui::Text("Near Plane Mouse X: %.2f", normalized_x);
			ImGui::Text("Near Plane Mouse Y: %.2f", normalized_y);
		}

		if (ImGui::CollapsingHeader("Time"))
		{
			ImGui::Text("Frame Count: %d", Time::frameCount);

			ImGui::Spacing();
			ImGui::Separator();

			ImGui::Text("Real Time");
			ImGui::Spacing();
			ImGui::Text("Delta Time: %.3f", Time::realClock.deltaTime);
			ImGui::Text("Time Since Startup %.3f", Time::realClock.timeSinceStartup());

			ImGui::Spacing();
			ImGui::Separator();

			ImGui::Text("Game Time");
			ImGui::Spacing();
			ImGui::Text("Delta time %.3f", Time::gameClock.deltaTime);
			ImGui::Text("Time Scale: %.2f", Time::gameClock.timeScale);
			ImGui::Text("Time since game start: %.2f", Time::gameClock.timeSinceStartup());
		}

		if (ImGui::CollapsingHeader("Resources"))
		{
			ImGui::Checkbox("Delete Library at shutdown", &engine->resources->cleanLibrary);
			ImGui::Checkbox("Delete metas at shutdown", &engine->resources->cleanMetas);
		}

		/*
		if (ImGui::CollapsingHeader("File System"))
		{
			ImGui::Checkbox("Normalize imported meshes", &FileSystem::normalize_scales);
		}
		*/

	}
	ImGui::End();
}

void WindowConfiguration::GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved)
{
	if (strcmp(gpu_brand, "NVIDIA Corporation") == 0)
	{
		/*glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available);
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);*/
	}
	else if (strcmp(gpu_brand, "ATI Technologies") == 0)
	{
		//glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		vram_budget = -1;
		//glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram_usage);
		vram_usage = -1;
		//glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, &vram_available);
		//glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &vram_reserved);
		vram_reserved = -1;
	}
}

void ModuleEditor::ShowPreferencesWindow()
{
	if (ImGui::Begin("Preferences", &show_preferences_window)) {
		//Style
		const char* items[] = { "Classic", "Dark", "Light" };
		if (ImGui::Combo("Interface Style", &current_theme, items, IM_ARRAYSIZE(items)))
		{
			ChangeTheme(std::string(items[current_theme]));
		}
	}
	ImGui::End();
}


