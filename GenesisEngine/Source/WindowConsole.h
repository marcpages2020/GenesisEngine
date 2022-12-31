#ifndef _WINDOW_CONSOLE_H_
#define _WINDOW_CONSOLE_H_

#include <vector>
#include <string>

#include "EditorWindow.h"

enum class LogType;

struct LogMessage 
{
	LogMessage(LogType type, std::string messageText);

	LogType logType;
	std::string message;
};

class WindowConsole : public EditorWindow {
public:
	WindowConsole();
	~WindowConsole();
	void Draw() override;
	void AddConsoleLog(LogType logType, const char* log);

private:
	std::vector<LogMessage> consoleLog;
	bool show_normal_log;
	bool show_warnings;
	bool show_errors;
};

#endif // !_WINDOW_CONSOLE_H_

