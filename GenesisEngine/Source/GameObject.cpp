#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "ImGui/imgui.h"

#include <vector>

GameObject::GameObject() : enabled(true), name("Empty Game Object"), parent(nullptr), to_delete(false)
{
	transform = (Transform*)AddComponent(ComponentType::TRANSFORM);
}

GameObject::GameObject(GnMesh* mesh) : GameObject()
{
	SetName(mesh->name);
	AddComponent((Component*)mesh);
}

GameObject::~GameObject(){
	parent = nullptr;

	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	transform = nullptr;

	components.clear();
	children.clear();
	name.clear();
}

void GameObject::Update()
{
	if (enabled)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->IsEnabled())
				components[i]->Update();
		}

		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}
}

void GameObject::OnEditor()
{
	ImGui::Checkbox("Enabled", &enabled);
	ImGui::SameLine();
	ImGui::Text(name.c_str());
	//static char buf[64] = "Name";
	//strcpy(buf, name.c_str());
	//if (ImGui::InputText("", &buf[0], IM_ARRAYSIZE(buf))){}

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnEditor();
	}

	if(ImGui::CollapsingHeader("Debugging Information")) 
	{
		if(parent != nullptr)
			ImGui::Text("Parent: %s", parent->GetName());
		else 
			ImGui::Text("No parent");
	}
}

Component* GameObject::GetComponent(ComponentType component)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == component)
		{
			return components[i];
		}
	}

	return nullptr;
}

Component* GameObject::AddComponent(ComponentType type)
{
	if (type == ComponentType::TRANSFORM) 
	{
		Transform* transform = new Transform();
		transform->SetGameObject(this);
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
	component->SetGameObject(this);
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

const char* GameObject::GetName() { return name.c_str(); }

void GameObject::SetName(const char* g_name) 
{
	name = g_name;
}

void GameObject::SetTransform(Transform g_transform)
{
	//localTransform->Set(g_transform.GetLocalTransform());
	//localTransform->UpdateLocalTransform();
	memcpy(transform, &g_transform, sizeof(g_transform));
}

Transform* GameObject::GetTransform()
{
	return transform;
}

void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
}

int GameObject::GetChildAmount()
{
	return children.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return children[index];
}

GameObject* GameObject::GetParent()
{
	return parent;
}

void GameObject::SetParent(GameObject* g_parent)
{
	parent = g_parent;
}

bool GameObject::RemoveChild(GameObject* gameObject)
{
	bool ret = false;
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i] == gameObject)
		{
			children.erase(children.begin() + i);
			ret = true;
		}
	}
	return ret;
}

void GameObject::DeleteChildren()
{
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->DeleteChildren();
		children[i] = nullptr;
	}

	this->~GameObject();
}

void GameObject::UpdateChildrenTransforms()
{
	transform->UpdateLocalTransform();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->GetTransform()->UpdateGlobalTransform(transform->GetGlobalTransform());
	}
}
