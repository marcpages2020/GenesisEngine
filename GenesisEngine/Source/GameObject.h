#pragma once

#include "Globals.h"

#include <vector>
#include <string>

#include "MathGeoLib/include/MathGeoLib.h"

class Component;
class Transform;
class GnMesh;
enum ComponentType;

class GnJSONObj;
class GnJSONArray;

class GameObject {
public:
	GameObject();
	GameObject(ComponentType component);
	~GameObject();

	void Update();
	void OnEditor();

	void Save(GnJSONArray& save_array);
	uint Load(GnJSONObj* object);
	uint LoadNodeData(GnJSONObj* object);

	Component* GetComponent(ComponentType component);
	std::vector<Component*> GetComponents();
	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);
	bool RemoveComponent(Component* component);

	const char* GetName();
	void SetName(const char* name);
	void SetTransform(Transform transform);
	Transform* GetTransform();
	AABB GetAABB();
	bool IsVisible();

	GameObject* GetParent();
	void SetParent(GameObject* parent);
	void Reparent(GameObject* newParent);

	void AddChild(GameObject* child);
	int GetChildrenAmount();
	GameObject* GetChildAt(int index);
	bool RemoveChild(GameObject* gameObject);
	void DeleteChildren();
	void UpdateChildrenTransforms();

public: 
	bool to_delete;
	uint UUID = 0;

private:
	void GenerateAABB(GnMesh* mesh);

private:
	bool enabled;
	bool _visible;
	std::string name;
	GameObject* _parent;
	Transform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> children;

	OBB _OBB;
	AABB _AABB;
};
