#pragma once

#include <vector>
#include <string>

class Component;
enum class ComponentType;
class Transform;

class GameObject {
public:
	GameObject();
	~GameObject();

	void Update();
	void OnEditor();

	Component* GetComponent(ComponentType component);
	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);
	bool RemoveComponent(Component* component);

	const char* GetName();
	void SetName(const char* name);
	void SetTransform(Transform transform);
	Transform* GetTransform();

	void AddChild(GameObject* child);
	int GetChildAmount();
	GameObject* GetChildAt(int index);
	void SetParent(GameObject* parent);

private:
	bool enabled;
	std::string name;
	GameObject* parent;
	Transform* transform;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
};
