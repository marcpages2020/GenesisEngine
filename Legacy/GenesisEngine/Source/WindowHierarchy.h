#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowHierarchy : public EditorWindow {
public:
	WindowHierarchy();
	~WindowHierarchy();

	void Draw() override;
	void PreorderHierarchy(GameObject* gameObject, int& id);
};
