#pragma once

class Application;

class GnJSONObj;

class Module
{
private :
	bool enabled;

public: 
	const char* name = "No Name Module";

public:

	Module(bool start_enabled = true) 
	{
		enabled = true;
	}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool LoadConfig(GnJSONObj& object)
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual void OnFrameEnd() {}

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};