#pragma once
#include "Module.h"
#include "Globals.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	bool Init();
	bool LoadConfig(JSON_Object* config) override;
	update_status Update(float dt);
	bool CleanUp();

	void AddGameObject(GameObject* gameObject);
	GameObject* GetRoot() { return root; }

public:
	bool show_grid;
	GameObject* selected_game_object;

private:
	GameObject* root;
	std::vector<GameObject*> gameObjects;
};
