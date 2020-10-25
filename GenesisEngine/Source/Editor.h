#ifndef _EDITOR_H_
#define _EDITOR_H_

#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"

#include <vector>
#include <string>

typedef int GLint;

enum class AspectRatio {
	FREE_ASPECT,
	ASPECT_16_9,
	ASPECT_4_3
};

struct log_message {
	std::string log_text;
	int warning_level;
};

class Editor : public Module 
{
public:
	Editor(bool start_enabled = true);
	~Editor();

	bool Init();
	bool LoadConfig(JSON_Object* config) override;
	update_status Update(float dt);
	update_status Draw();
	bool CleanUp();

	bool IsSceneFocused();
	void AddConsoleLog(const char* log, int warning_level);
	
private:
	//Dock Space
	update_status ShowDockSpace(bool* p_open);
	bool CreateMainMenuBar();

	//Windows
	void ShowSceneWindow();
	void ShowInspectorWindow();
	void ShowHierarchyWindow();
	void ShowConfigurationWindow();
	void ShowAboutWindow();

	void ChangeTheme(std::string theme);
	void GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved);

	void ResizeSceneImage(ImVec2 window_size, AspectRatio ratio);
	void PreorderHierarchy(GameObject* gameObject);

private:
	bool show_inspector_window;
	bool show_project_window;
	bool show_hierarchy_window;
	bool show_console_window;
	bool show_scene_window;
	bool show_configuration_window;

	//edit subwindows
	bool show_preferences_window;
	bool show_about_window;
	//menus
	bool* open_dockspace;
	bool scene_window_focused;

	int current_theme;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	std::vector<log_message> console_log;

	ImVec2 image_size;

	AspectRatio aspect_ratio;
};

#endif // !_EDITOR_H_

