#pragma once
#include "Module.h"
#include "Globals.h"

#include <vector>

class GameObject;
class GnTexture;

class ModuleScene : public Module
{
public:
	ModuleScene(bool start_enabled = true);
	~ModuleScene();

	bool Start();
	bool Init();
	bool LoadConfig(GnJSONObj& config) override;
	update_status Update(float dt);
	bool CleanUp();

	void AddGameObject(GameObject* gameObject);
	void DeleteGameObject(GameObject* gameObject);
	GameObject* GetRoot() { return root; }
	std::vector<GameObject*> GetAllGameObjects();
	void PreorderGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects);
	void SetDroppedTexture(GnTexture* texture);

	bool ClearScene();

	bool Save();
	bool Load(const char* scene_file);

public:
	bool show_grid;
	GameObject* selectedGameObject;

private:
	GameObject* root;
};
