#include "WindowConsole.h"
#include "ImGui/imgui.h"

WindowConsole::WindowConsole() : show_normal_log(true), show_warnings(true), show_errors(true)
{
	type = WindowType::WINDOW_CONSOLE;
}

WindowConsole::~WindowConsole()
{
	console_log.clear();
}

void WindowConsole::Draw()
{
	if (ImGui::Begin("Console", &visible, ImGuiWindowFlags_MenuBar)) {

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Clear"))
			{
				console_log.clear();
				visible_log.clear();
			}

			if (ImGui::Checkbox("Log", &show_normal_log))
			{
				FilterLog();
			}

			if (ImGui::Checkbox("Warnings", &show_warnings))
			{
				FilterLog();
			}

			if (ImGui::Checkbox("Errors", &show_errors))
			{
				FilterLog();
			}

			ImGui::EndMenuBar();
		}

		for (int i = 0; i < visible_log.size(); i++)
		{
			if (visible_log[i].warning_level == 0) //Normal log
			{
				ImGui::Text(visible_log[i].log_text.c_str());
			}
			else if (visible_log[i].warning_level == 1) //Warning
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), visible_log[i].log_text.c_str());
			}
			else //Error
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), visible_log[i].log_text.c_str());
		}
	}
	ImGui::End();
}

void WindowConsole::AddConsoleLog(const char* log, int warning_level)
{
	log_message message = { log, warning_level };
	console_log.push_back(message);

	switch (warning_level)
	{
	case 0: //Normal Log
		if (show_normal_log)
		{
			visible_log.push_back(message);
		}
		break;
	case 1: //Warnings
		if (show_warnings)
		{
			visible_log.push_back(message);
		}
		break;
	case 2: //Errors
		if (show_errors)
		{
			visible_log.push_back(message);
		}
		break;
	default:
		break;
	}
}

void WindowConsole::FilterLog()
{
	visible_log.clear();

	for (size_t i = 0; i < console_log.size(); i++)
	{
		switch (console_log[i].warning_level)
		{
		case 0: //Normal Log
			if (show_normal_log)
			{
				visible_log.push_back(console_log[i]);
			}
			break;
		case 1: //Warnings
			if (show_warnings)
			{
				visible_log.push_back(console_log[i]);
			}
			break;
		case 2: //Errors
			if (show_errors)
			{
				visible_log.push_back(console_log[i]);
			}
			break;
		default:
			break;
		}
	}
}
