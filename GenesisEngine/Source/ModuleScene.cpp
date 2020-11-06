#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GnJSON.h"
#include "Mesh.h"
#include "FileSystem.h"
#include "GameObject.h"

ModuleScene::ModuleScene(bool start_enabled) : Module(start_enabled), show_grid(true), selectedGameObject(nullptr), root(nullptr) 
{
	name = "scene";
}

ModuleScene::~ModuleScene() {}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	root = new GameObject();
	selectedGameObject = root;
	root->SetName("Root");

	GameObject* house = MeshImporter::ImportFBX("Assets/Models/baker_house/BakerHouse.FBX");
	AddGameObject(house);

	//GameObject* rayman = MeshImporter::ImportFBX("Assets/Models/rayman/rayman.fbx");
	//AddGameObject(rayman);

	//MeshImporter::ImportFBX("Assets/Models/monkey.fbx");

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

	root->DeleteChildren();
	delete root;
	root = nullptr;

	selectedGameObject = nullptr;

	return true;
}

void ModuleScene::AddGameObject(GameObject* gameObject)
{
	gameObject->SetParent(root);
	root->AddChild(gameObject);

	selectedGameObject = gameObject;
}

void ModuleScene::DeleteGameObject(GameObject* gameObject)
{
	if (root->RemoveChild(gameObject))
	{
		gameObject->DeleteChildren();
	}
	else if (gameObject->GetParent()->RemoveChild(gameObject))
	{
		gameObject->DeleteChildren();
	}

	delete gameObject;
	gameObject = nullptr;
}

void ModuleScene::SetDroppedTexture(GnTexture* texture)
{
	if (selectedGameObject != nullptr) 
	{
		if (selectedGameObject->GetComponent(ComponentType::MESH) == nullptr && texture != nullptr) 
		{
			delete texture;
			texture = nullptr;
			return;
		}

		Material* material = dynamic_cast<Material*>(selectedGameObject->GetComponent(ComponentType::MATERIAL));

		if (material == nullptr)
		{
			material = dynamic_cast<Material*>(selectedGameObject->AddComponent(ComponentType::MATERIAL));
		}

		material->SetTexture(texture);
	}
}

bool ModuleScene::Save()
{
	bool ret = true;

	GnJSONObj save_file;

	save_file.AddString("name", "Save File");

	GnJSONArray gameObjects = save_file.AddArray("Game Objects");

	root->Save(gameObjects);

	char* buffer = NULL;
	uint size = save_file.Save(&buffer);

	FileSystem::Save("Library/Config/save_file.save", buffer, size);

	return ret;
}

bool ModuleScene::LoadConfig(GnJSONObj& config)
{
	show_grid = config.GetBool("show_grid");

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

	root->Update();

	if ((App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) && (selectedGameObject != nullptr) && (selectedGameObject != root)) 
	{
		selectedGameObject->to_delete = true;
	}

	return UPDATE_CONTINUE;
}


