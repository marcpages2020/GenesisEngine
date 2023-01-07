#pragma once

class EditorWindow {
public:
	EditorWindow();
	virtual ~EditorWindow();
	virtual bool Init() { return true; };
	virtual void Draw() = 0;

	const char* GetName();

protected: 
	const char* name;

public:
	bool visible;
	bool focused;
};
