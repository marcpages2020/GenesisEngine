#pragma once

#include <vector>

class Component;
enum class ComponentType;

class GameObject {
public:
	GameObject();
	~GameObject();
	void Update();
	Component* AddComponent(ComponentType type);
	void AddComponent(Component* component);

	bool RemoveComponent(Component* component);

private:
	bool enabled;
	const char* name;
	GameObject* parent;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
};
