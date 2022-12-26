#pragma once

#include "EditorWindow.h"
#include <vector>

enum class LogType;

struct LogMessage
{
	LogMessage(LogType newType, char* newMessage);
	LogType logType;
	char* message;
};

class EditorWindow_Console : public EditorWindow
{
public:
	EditorWindow_Console(ModuleEditor* moduleEditor);

	void Draw() override;

	void AddConsoleLog(LogType logType, char* logMessage);

private:
	std::vector<LogMessage> logMessages;
};

