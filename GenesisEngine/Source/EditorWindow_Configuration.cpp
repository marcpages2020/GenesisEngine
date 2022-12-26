#include "EditorWindow_Configuration.h"
#include <ImGui/imgui.h>

#include "Engine.h"
#include "ModuleEditor.h"

EditorWindow_Configuration::EditorWindow_Configuration(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor)
{
	name = "Configuration";
	isOpen = true;

	FPSLog	= std::vector<float>(100, 0);
	msLog	= std::vector<float>(100, 0.0f);
}

void EditorWindow_Configuration::Draw()
{
	FPSLog.push_back(editor->engine->GetCurrentFPS());
	FPSLog.erase(FPSLog.begin());

	msLog.push_back(editor->engine->GetLastDeltaTime() * 1000.0f);
	msLog.erase(msLog.begin());

	if (ImGui::Begin(name, &isOpen)) 
	{
		if (ImGui::CollapsingHeader("Engine"))
		{
			//FPS Limiter
			int maxFPS = editor->engine->GetMaxFPS();
			if (ImGui::SliderInt("Max FPS", &maxFPS, 10, 120)) { editor->engine->SetMaxFPS(maxFPS);	}

			//FPS Log
			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", FPSLog[FPSLog.size() - 1]);
			ImGui::PlotHistogram("##framerate", &FPSLog[0], FPSLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %.1f", msLog[msLog.size() - 1]);
			ImGui::PlotHistogram("##milliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			//Brightness
			float windowBrightness = editor->engine->window->GetBrightness();
			if(ImGui::SliderFloat("Brightness", &windowBrightness, 0.0f, 1.0f)){
				editor->engine->window->SetBrightness(windowBrightness);}

			int windowWidth;  //= editor->engine->window->GetWidht();
			int windowHeight; // = editor->engine->window->GetHeight();
			editor->engine->window->GetSize(windowWidth, windowHeight);

			if ((ImGui::SliderInt("Width", &windowWidth, 640, 3840) || ImGui::SliderInt("Height", &windowHeight, 360, 2160))) {
				editor->engine->window->SetSize(windowWidth, windowHeight);}


			bool fullscreen = editor->engine->window->IsFullscreen();
			if (ImGui::Checkbox("Fullscreen", &fullscreen))
				editor->engine->window->SetFullscreen(fullscreen);

			ImGui::SameLine();

			static bool resizable = editor->engine->window->IsResizable();
			if (ImGui::Checkbox("Resizable", &resizable))
				editor->engine->window->SetResizable(resizable);

			static bool borderless = editor->engine->window->IsBorderless();
			if (ImGui::Checkbox("Borderless", &borderless))
				editor->engine->window->SetBorderless(borderless);

			ImGui::SameLine();

			bool fullscreenDesktop = editor->engine->window->IsFullscreenDesktop();
			if (ImGui::Checkbox("Fullscreen Desktop", &fullscreenDesktop))
				editor->engine->window->SetFullscreenDesktop(fullscreenDesktop);

		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImVec4 valuesColor(1.0f, 1.0f, 0.0f, 1.0f);

			//CPU
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%d (Cache: %dkb)", editor->engine->hardware->GetCPUCount(), editor->engine->hardware->GetCPUCache());

			//RAM
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Gb", ((float)editor->engine->hardware->GetSystemRAM()) / 1000.0f);

			//CAPS
			char char_caps[128];
			editor->engine->hardware->GetAllCaps(char_caps);
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s ", char_caps);

			ImGui::Separator();

			//GPU
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s", editor->engine->hardware->GetGPU());

			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s", editor->engine->hardware->GetGPUBrand());

			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->engine->hardware->GetVRAMBudget());

			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->engine->hardware->GetVRAMUsage());

			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->engine->hardware->GetVRAMAvailable());

			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->engine->hardware->GetVRAMReserved());
		}
	}
	ImGui::End();
}
