#pragma once

class Application;

struct json_object_t;
typedef struct json_object_t JSON_Object;

class Module
{
private :
	bool enabled;

public: 
	const char* name;

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

	virtual bool LoadConfig(JSON_Object* object)
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