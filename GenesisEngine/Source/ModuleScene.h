#pragma once
#include "Module.h"
#include "Globals.h"

class GameObject;
class GnTexture;

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
	void DeleteGameObject(GameObject* gameObject);
	GameObject* GetRoot() { return root; }
	void SetDroppedTexture(GnTexture* texture);

public:
	bool show_grid;
	GameObject* selectedGameObject;

private:
	GameObject* root;
};
