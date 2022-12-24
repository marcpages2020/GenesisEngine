#pragma once

class GnEngine;
class GnJSONObj;

class Module
{
private :
	bool enabled;

protected:
	const char* name = "None";

public:

	GnEngine* Engine;

	Module(GnEngine* parent, bool start_enabled = true) : Engine(parent)
	{
		enabled = true;
	}

	virtual ~Module()
	{}

	virtual bool Init()
	{
		return true; 
	}

	virtual bool LoadEditorConfig(GnJSONObj& object) { return true; }

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float deltaTime)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float deltaTime)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float deltaTime)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	const char* GetName() const { return name; }
};