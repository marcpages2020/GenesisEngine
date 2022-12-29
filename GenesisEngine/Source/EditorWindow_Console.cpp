#include "EditorWindow_Console.h"
#include "Globals.h"
#include <ImGui/imgui.h>

LogMessage::LogMessage(LogType newType, char* newMessage)
{
	logType = newType;
	message = newMessage;
}

EditorWindow_Console::EditorWindow_Console(ModuleEditor* moduleEditor) : EditorWindow(moduleEditor),
showNormalLog(true), showWarningLog(true), showErrorLog(true)
{
	isOpen = true;
	name = "Console";
}

void EditorWindow_Console::Draw()
{
	if (ImGui::Begin(name, &isOpen, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Clear")) {
				logMessages.clear();
			}

			ImGui::Checkbox("Log", &showNormalLog);
			ImGui::Checkbox("Warnings", &showWarningLog);
			ImGui::Checkbox("Errors", &showErrorLog);

			ImGui::EndMenuBar();
		}

		for (size_t i = 0; i < logMessages.size(); i++)
		{
			switch (logMessages[i].logType)
			{
			case LogType::LOG_NORMAL:
				if (showNormalLog) {
					ImGui::Text(logMessages[i].message);
				}
				break;
			case LogType::LOG_WARNING:
				if (showWarningLog) {
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), logMessages[i].message);
				}
				break;
			case LogType::LOG_ERROR:
				if (showErrorLog) {
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), logMessages[i].message);
				}
				break;

			default: break;
			}
		}
	}
	ImGui::End();
}

void EditorWindow_Console::AddConsoleLog(LogType logType, char* logMessage)
{
	char* message = new char[strlen(logMessage)];
	strcpy(message, logMessage);
	logMessages.push_back(LogMessage(logType, message));
}
