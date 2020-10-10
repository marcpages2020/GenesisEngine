#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "parson/parson.h"
#include "Mesh.h"

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

bool ModuleSceneIntro::Init(JSON_Object* object)
{
	show_grid = json_object_get_boolean(object, "show_grid");
	return true;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	bool wired = (App->renderer3D->GetDisplayMode() == WIREFRAME);

	if (show_grid) 
	{
		//Plane p(0, 1, 0, 0);
		//p.axis = true;
		//p.Render(wired);
	}
	
	/*
	Cube cube(1.0f, 1.0f, 1.0f);
	cube.color = Color(255, 0, 0, 255);
	cube.wire = true;
	cube.Render(wired);
	*/

	Grid grid(10);
	grid.Render();

	//Cube cube;
	//cube.Render();

	//Pyramid pyramid;
	//pyramid.Render();

	//SimplePlane plane;
	//plane.Render();

	//Sphere sphere;
	//sphere.Render();

	Cylinder cylinder(1, 2, 12);
	cylinder.Render();


	return UPDATE_CONTINUE;
}


