#ifndef _WINDOW_CONSOLE_H_
#define _WINDOW_CONSOLE_H_

#include <vector>
#include <string>

#include "EditorWindow.h"

struct log_message {
	std::string log_text;
	int warning_level;
};

class WindowConsole : public EditorWindow {
public:
	WindowConsole();
	~WindowConsole();
	void Draw() override;
	void AddConsoleLog(const char* log, int warning_level);

private:
	void FilterLog();

private:
	std::vector<log_message> console_log;
	std::vector<log_message> visible_log;
	bool show_normal_log;
	bool show_warnings;
	bool show_errors;
};

#endif // !_WINDOW_CONSOLE_H_

