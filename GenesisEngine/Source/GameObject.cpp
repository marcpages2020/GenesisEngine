#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "ImGui/imgui.h"

#include <vector>

GameObject::GameObject() : enabled(true), name("Empty Game Object"), parent(nullptr)
{
	AddComponent(ComponentType::TRANSFORM);
}

GameObject::~GameObject(){
	parent = nullptr;

	//TODO: CleanUp Components
	components.clear();
	children.clear();
}

void GameObject::Update()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

void GameObject::OnEditor()
{
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SameLine();
	ImGui::Text(name);
	//static char nameBuffer[256] = { *name };
	//ImGui::InputText(" ", nameBuffer, IM_ARRAYSIZE(nameBuffer));

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnEditor();
	}
}

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
