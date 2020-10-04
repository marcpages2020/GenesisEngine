#pragma once

class Application;

#include <string>

//struct json_array_t;
//typedef struct json_array_t JSON_Array;

class Module
{
private :
	bool enabled;

public: 
	std::string name;

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

	virtual bool CleanUp() 
	{ 
		return true; 
	}
};