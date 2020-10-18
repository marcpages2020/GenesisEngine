#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "parson/parson.h"
#include "Mesh.h"
#include "FileSystem.h"

//#include "glew/include/glew.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
	name = "scene";
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));

	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

bool ModuleSceneIntro::Init()
{
	return true;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

bool ModuleSceneIntro::LoadConfig(JSON_Object* config)
{
	show_grid = json_object_get_boolean(config, "show_grid");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	if (show_grid) 
	{
		Grid grid(10);
		grid.Render();
	}

	//Cube cube;
	//cube.Render();;

	//Pyramid pyramid;
	//pyramid.Render();

	//Plane plane;
	//plane.Render();

	//Sphere sphere;
	//sphere.Render();

	//Cylinder cylinder(1, 2, 16);
	//cylinder.Render();

	//Cone cone(1, 1.5, 12);
	//cone.Render();

	return UPDATE_CONTINUE;
}


