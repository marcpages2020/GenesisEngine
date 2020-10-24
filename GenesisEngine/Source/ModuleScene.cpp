#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "parson/parson.h"
#include "Mesh.h"
#include "FileSystem.h"
#include "GameObject.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled), show_grid(true), selected_game_object(nullptr), root(nullptr) 
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

	gameObjects.push_back(root);
	selected_game_object = root;
	root->SetName("Root");

	//GameObject* house = new GameObject();
	GameObject* house = FileSystem::LoadFBX("Assets/Models/baker_house/BakerHouse.FBX");
	//AddGameObject(house);
	//house->AddComponent(FileSystem::LoadFBX("Assets/Models/baker_house/BakerHouse.FBX"));
	//house->AddComponent(ComponentType::MATERIAL);
	//AddGameObject(house);

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

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
		gameObjects[i] = nullptr;
	}

	root = nullptr;
	gameObjects.clear();

	return true;
}

void ModuleScene::AddGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);

	gameObject->SetParent(root);
	
	root->AddChild(gameObject);

	selected_game_object = gameObject;
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


