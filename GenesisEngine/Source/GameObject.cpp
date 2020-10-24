#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
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

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->Update();
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
	else if (type == ComponentType::MESH)
	{
		GnMesh* mesh = new GnMesh();
		components.push_back(mesh);
		return mesh;
	}
	else if (type == ComponentType::MATERIAL)
	{
		Material* material = new Material();
		components.push_back(material);
		return material;
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

const char* GameObject::GetName() { return name; }

void GameObject::SetName(const char* g_name) { name = (char* )g_name ; }

void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
}

void GameObject::SetParent(GameObject* g_parent)
{
	parent = g_parent;
}
