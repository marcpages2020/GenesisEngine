#pragma once

class EditorWindow
{
public: 
	EditorWindow();

	virtual void Draw() = 0;

	void SetOpen(bool open);
	bool IsOpen() const;
	char* GetName() const;

protected: 
	char* name;

	bool isOpen;
};