#pragma once

enum class EditorWindowType
{
	NONE,
	ASSETS,
	SCENE,
	CONFIGURATION,
	MAX,
};

class EditorWindow
{
public: 
	EditorWindow();

	virtual void Draw();

	bool IsOpen();

protected: 
	bool isOpen;
	EditorWindowType windowType;
};