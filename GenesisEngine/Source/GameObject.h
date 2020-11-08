#pragma once

#include "Globals.h"

#include <vector>
#include <string>

class Component;
class Transform;
class GnMesh;
enum ComponentType;

class GnJSONObj;
class GnJSONArray;

class GameObject {
public:
	GameObject();
	GameObject(GnMesh* mesh);
	~GameObject();

	void Update();
	void OnEditor();

	void Save(GnJSONArray& save_array);
	uint Load(GnJSONObj* object);

	Component* GetComponent(ComponentType component);
	std::vector<Component*> GetComponents();
	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);
	bool RemoveComponent(Component* component);

	const char* GetName();
	void SetName(const char* name);
	void SetTransform(Transform transform);
	Transform* GetTransform();

	GameObject* GetParent();
	void SetParent(GameObject* parent);

	void AddChild(GameObject* child);
	int GetChildAmount();
	GameObject* GetChildAt(int index);
	bool RemoveChild(GameObject* gameObject);
	void DeleteChildren();
	void UpdateChildrenTransforms();

public: 
	bool to_delete;
	uint UUID = -1;

private:
	bool enabled;
	std::string name;
	GameObject* parent;
	Transform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
};
