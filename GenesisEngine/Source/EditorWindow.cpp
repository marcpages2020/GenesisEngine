#include "EditorWindow.h"

EditorWindow::EditorWindow() : isOpen(false), windowType(EditorWindowType::NONE)
{
	
}

void EditorWindow::Draw()
{

}

bool EditorWindow::IsOpen()
{
	return isOpen;
}
