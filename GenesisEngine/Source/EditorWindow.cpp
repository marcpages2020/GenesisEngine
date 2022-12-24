#include "EditorWindow.h"

EditorWindow::EditorWindow(ModuleEditor* moduleEditor) : editor(moduleEditor), isOpen(false), name("Unnamed  Window")
{}

EditorWindow::~EditorWindow()
{
	delete name;
}

void EditorWindow::SetOpen(bool open)
{ 
	isOpen = open;
}

bool EditorWindow::IsOpen() const
{
	return isOpen;
}

char* EditorWindow::GetName() const
{
	return name;
}


