#include "WindowConsole.h"
#include "ImGui/imgui.h"

#include "Globals.h"

WindowConsole::WindowConsole() : show_normal_log(true), show_warnings(true), show_errors(true)
{
	type = WindowType::WINDOW_CONSOLE;
}

WindowConsole::~WindowConsole()
{
	consoleLog.clear();
}

void WindowConsole::Draw()
{
	if (ImGui::Begin("Console", &visible, ImGuiWindowFlags_MenuBar)) {

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Clear")) {
				consoleLog.clear();
			}

			ImGui::Checkbox("Log", &show_normal_log);
			ImGui::Checkbox("Warnings", &show_warnings);
			ImGui::Checkbox("Errors", &show_errors);			

			ImGui::EndMenuBar();
		}

		for (int i = 0; i < consoleLog.size(); ++i)
		{
			switch (consoleLog[i].logType)
			{
			case LogType::LOG_NORMAL: 
				if (show_normal_log) {
					ImGui::Text(consoleLog[i].message.c_str());
				}
				break;
			case LogType::LOG_WARNING:
				if (show_warnings) {
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), consoleLog[i].message.c_str());
				}
				break;
			case LogType::LOG_ERROR:
				if (show_errors) {
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), consoleLog[i].message.c_str());
				}
				break;
			default:
				break;
			}
		}
	}
	ImGui::End();
}

void WindowConsole::AddConsoleLog(LogType logType, const char* log)
{
	LogMessage message = { logType, log };
	consoleLog.push_back(message);
}

LogMessage::LogMessage(LogType type, std::string messageText) : logType(type), message(messageText) 
{}
