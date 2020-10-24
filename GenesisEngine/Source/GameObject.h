#pragma once

#include <vector>

class Component;
enum class ComponentType;

class GameObject {
public:
	GameObject();
	~GameObject();

	void Update();
	void OnEditor();

	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);
	bool RemoveComponent(Component* component);

	const char* GetName();
	void SetName(const char* name);

	void AddChild(GameObject* child);
	void SetParent(GameObject* parent);

private:
	bool enabled;
	char* name;
	GameObject* parent;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
};
