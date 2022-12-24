#pragma once

class GnEngine;

class Module
{
private :
	bool enabled;

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
};