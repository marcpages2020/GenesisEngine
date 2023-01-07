#include "EditorWindow.h"

EditorWindow::EditorWindow() : visible(false), focused(false), name("No name")
{}

EditorWindow::~EditorWindow()
{}

const char* EditorWindow::GetName()
{
    return name;
}
