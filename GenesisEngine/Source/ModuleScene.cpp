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

	//GameObject* house = 
	//uint baker_house = App->resources->ImportFile("Assets/Models/baker_house/BakerHouse.FBX");
	uint baker_house_uid = App->resources->Find("Assets/Models/baker_house/BakerHouse.FBX");
	//App->resources->Find(baker_house_uid);
	Resource* baker_house_resource = App->resources->RequestResource(baker_house_uid);
	//uint baker_house_texture = App->resources->ImportFile("Assets/Textures/Baker_house.png");
	//AddGameObject(house);

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

		//material->SetTexture(texture);
	}
}

bool ModuleScene::ClearScene()
{
	bool ret = true;

	root->DeleteChildren();
	root = nullptr;

	return ret;
}

bool ModuleScene::Save()
{
	bool ret = true;

	GnJSONObj save_file;

	GnJSONArray gameObjects = save_file.AddArray("Game Objects");

	root->Save(gameObjects);

	char* buffer = NULL;
	uint size = save_file.Save(&buffer);

	FileSystem::Save("Library/Scenes/new_scene.scene", buffer, size);

	save_file.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

bool ModuleScene::Load(const char* scene_file)
{
	bool ret = true;

	ClearScene();

	char* buffer = NULL;
	FileSystem::Load(scene_file, &buffer);
	
	GnJSONObj base_object(buffer);
	GnJSONArray gameObjects(base_object.GetArray("Game Objects"));

	std::vector<GameObject*> createdObjects;

	for (size_t i = 0; i < gameObjects.Size(); i++)
	{
		//load game object
		GameObject* gameObject = new GameObject();
		uint parentUUID = gameObject->Load(&gameObjects.GetObjectAt(i));
		createdObjects.push_back(gameObject);

		//check if it's the root game object
		if (strcmp(gameObject->GetName(), "Root") == 0) {
			root = gameObject;
			selectedGameObject = root;
		}

		//Get game object's parent
		for (size_t i = 0; i < createdObjects.size(); i++)
		{
			if (createdObjects[i]->UUID == parentUUID)
			{
				createdObjects[i]->AddChild(gameObject);
				gameObject->SetParent(createdObjects[i]);
			}
		}
	}

	if (root != nullptr)
		LOG("Scene loaded successfully");

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


