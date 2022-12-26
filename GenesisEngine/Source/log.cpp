#pragma once
#include "Globals.h"
#include "Engine.h"
#include "EditorWindow_Console.h"

void log(LogType logType, const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	GnEngine* engine = GnEngine::Instance();
	if (engine && engine->editor)
	{
		EditorWindow_Console* console = (EditorWindow_Console*)engine->editor->GetWindowByName("Console");
		if (console)
		{
			console->AddConsoleLog(logType, tmp_string2);
		}
	}
}
