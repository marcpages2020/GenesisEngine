#include "WindowAbout.h"
#include "ImGui/imgui.h"
#include "Engine.h"
#include "glad/include/glad/glad.h"
#include "Assimp/Assimp/include/version.h"
#include "FileSystem.h"

WindowAbout::WindowAbout() : EditorWindow() 
{
	type = WindowType::WINDOW_ABOUT;
}

WindowAbout::~WindowAbout() {}

void WindowAbout::Draw()
{
	if (ImGui::Begin("About", &visible))
	{
		focused = ImGui::IsWindowFocused();
		ImGui::Text("%s v%s", engine->engine_name, engine->engine_version);
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
		//ImGui::BulletText("Glew %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);

		ImGui::BulletText("DevIL 1.8.0");

		ImGui::BulletText("Assimp %d.%d.%d", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

		static std::string physfs_version;
		FileSystem::GetPhysFSVersion(physfs_version);
		ImGui::BulletText("PhysFS %s", physfs_version.c_str());

		ImGui::BulletText("Parson 1.1.0");

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
