#include "EditorWindow.h"

EditorWindow::EditorWindow() : isOpen(false), name("Unnamed  Window")
{}

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


