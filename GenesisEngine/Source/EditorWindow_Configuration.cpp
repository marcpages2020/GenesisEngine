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
	FPSLog.push_back(editor->Engine->GetCurrentFPS());
	FPSLog.erase(FPSLog.begin());

	msLog.push_back(editor->Engine->GetLastDeltaTime() * 1000.0f);
	msLog.erase(msLog.begin());

	if (ImGui::Begin(name, &isOpen)) 
	{
		if (ImGui::CollapsingHeader("Engine"))
		{
			//FPS Limiter
			int maxFPS = editor->Engine->GetMaxFPS();
			if (ImGui::SliderInt("Max FPS", &maxFPS, 10, 120)) { editor->Engine->SetMaxFPS(maxFPS);	}

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
			float windowBrightness = editor->Engine->window->GetBrightness();
			if(ImGui::SliderFloat("Brightness", &windowBrightness, 0.0f, 1.0f)){
				editor->Engine->window->SetBrightness(windowBrightness);}

			int windowWidth;  //= editor->Engine->window->GetWidht();
			int windowHeight; // = editor->Engine->window->GetHeight();
			editor->Engine->window->GetSize(windowWidth, windowHeight);

			if ((ImGui::SliderInt("Width", &windowWidth, 640, 3840) || ImGui::SliderInt("Height", &windowHeight, 360, 2160))) {
				editor->Engine->window->SetSize(windowWidth, windowHeight);}


			bool fullscreen = editor->Engine->window->IsFullscreen();
			if (ImGui::Checkbox("Fullscreen", &fullscreen))
				editor->Engine->window->SetFullscreen(fullscreen);

			ImGui::SameLine();

			static bool resizable = editor->Engine->window->IsResizable();
			if (ImGui::Checkbox("Resizable", &resizable))
				editor->Engine->window->SetResizable(resizable);

			static bool borderless = editor->Engine->window->IsBorderless();
			if (ImGui::Checkbox("Borderless", &borderless))
				editor->Engine->window->SetBorderless(borderless);

			ImGui::SameLine();

			bool fullscreenDesktop = editor->Engine->window->IsFullscreenDesktop();
			if (ImGui::Checkbox("Fullscreen Desktop", &fullscreenDesktop))
				editor->Engine->window->SetFullscreenDesktop(fullscreenDesktop);

		}

		if (ImGui::CollapsingHeader("Hardware"))
		{
			ImVec4 valuesColor(1.0f, 1.0f, 0.0f, 1.0f);

			//CPU
			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%d (Cache: %dkb)", editor->Engine->hardware->GetCPUCount(), editor->Engine->hardware->GetCPUCache());

			//RAM
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Gb", ((float)editor->Engine->hardware->GetSystemRAM()) / 1000.0f);

			//CAPS
			char char_caps[128];
			editor->Engine->hardware->GetAllCaps(char_caps);
			ImGui::Text("Caps:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s ", char_caps);

			ImGui::Separator();

			//GPU
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s", editor->Engine->hardware->GetGPU());

			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%s", editor->Engine->hardware->GetGPUBrand());

			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->Engine->hardware->GetVRAMBudget());

			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->Engine->hardware->GetVRAMUsage());

			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->Engine->hardware->GetVRAMAvailable());

			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(valuesColor, "%.1f Mb", (float)editor->Engine->hardware->GetVRAMReserved());
		}
	}
	ImGui::End();
}
