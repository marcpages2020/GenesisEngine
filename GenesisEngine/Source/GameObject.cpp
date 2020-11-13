#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "ImGui/imgui.h"
#include "GnJSON.h"

#include "MathGeoLib/include/MathGeoLib.h"

#include <vector>

GameObject::GameObject() : enabled(true), name("Empty Game Object"), parent(nullptr), to_delete(false), transform(nullptr)
{
	transform = (Transform*)AddComponent(TRANSFORM);
	UUID = LCG().Int();
}

GameObject::GameObject(GnMesh* mesh) : GameObject()
{
	SetName(mesh->name);
	AddComponent((Component*)mesh);
}

GameObject::~GameObject()
{
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
	UUID = 0;
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

		ImGui::Text("UUID: %d", UUID);
	}
}

void GameObject::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("UUID", UUID);

	if(parent != nullptr)
		save_object.AddInt("Parent UUID",parent->UUID);

	save_object.AddString("Name", name.c_str());

	//math::float3 position = transform->GetPosition();
	//save_object.AddFloat3("Position", position);

	//math::Quat rotation = transform->GetRotation();
	//save_object.AddQuaternion("Rotation", rotation);

	//math::float3 scale = transform->GetScale();
	//save_object.AddFloat3("Scale", scale);

	GnJSONArray componentsSave = save_object.AddArray("Components");

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Save(componentsSave);
	}

	save_array.AddObject(save_object);

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->Save(save_array);
	}
}

uint GameObject::Load(GnJSONObj* object)
{
	UUID = object->GetInt("UUID");
	name = object->GetString("Name");
	uint parentUUID = object->GetInt("Parent UUID");

	GnJSONArray componentsArray = object->GetArray("Components");

	for (size_t i = 0; i < componentsArray.Size(); i++)
	{
		GnJSONObj componentObject = componentsArray.GetObjectAt(i);
		Component* component = AddComponent((ComponentType)componentObject.GetInt("Type"));
		component->Load(componentObject);
	}

	return parentUUID;
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

std::vector<Component*> GameObject::GetComponents()
{
	return components;
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;

	if (type == ComponentType::TRANSFORM) 
	{
		if (transform != nullptr)
		{
			RemoveComponent(transform);
		}

		transform = new Transform();
		component = transform;
	}
	else if (type == ComponentType::MESH)
	{
		component = new GnMesh();
	}
	else if (type == ComponentType::MATERIAL)
	{
		component = new Material(this);
	}

	component->SetGameObject(this);
	components.push_back(component);

	return component;
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
	transform->UpdateGlobalTransform();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->GetTransform()->UpdateGlobalTransform(transform->GetGlobalTransform());
	}
}
