#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "parson/parson.h"
#include "Mesh.h"
#include "FileSystem.h"
#include "GameObject.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled), show_grid(true), selected_game_object(nullptr)
{
	name = "scene";
}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));

	App->camera->LookAt(vec3(0, 0, 0));

	root = new GameObject();
	AddGameObject(root);
	selected_game_object = root;

	return ret;
}

bool ModuleScene::Init()
{
	return true;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	delete root;
	root = nullptr;
	gameObjects.clear();

	return true;
}

void ModuleScene::AddGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
}

bool ModuleScene::LoadConfig(JSON_Object* config)
{
	show_grid = json_object_get_boolean(config, "show_grid");

	return true;
}

// Update: draw background
update_status ModuleScene::Update(float dt)
{
	if (show_grid) 
	{
		GnGrid grid(24);
		grid.Render();
	}

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Update();
	}

	//GnCube cube;
	//cube.Render();;

	//GnPyramid pyramid;
	//pyramid.Render();

	//GnPlane plane;
	//plane.Render();

	//GnSphere sphere;
	//sphere.Render();

	//GnCylinder cylinder(1, 2, 16);
	//cylinder.Render();

	//GnCone cone(1, 1.5, 12);
	//cone.Render();

	return UPDATE_CONTINUE;
}


