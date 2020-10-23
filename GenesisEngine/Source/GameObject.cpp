#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

GameObject::GameObject() : enabled(true), name("Empty Game Object"), parent(nullptr)
{
	AddComponent(ComponentType::TRANSFORM);
}

GameObject::~GameObject(){}

void GameObject::Update(){}

Component* GameObject::AddComponent(ComponentType type)
{
	if (type == ComponentType::TRANSFORM) 
	{
		Transform* transform = new Transform();
		components.push_back(transform);
		return transform;
	}
	return nullptr;
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}

bool GameObject::RemoveComponent(Component* component)
{
	bool ret = false;

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i] == component) {
			delete components[i];
			components.erase(components.begin() + i);
			component = nullptr;
			ret = true;
		}
	}

	return ret;
}
