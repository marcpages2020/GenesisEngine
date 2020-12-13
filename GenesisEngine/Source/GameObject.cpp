#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "ImGui/imgui.h"
#include "GnJSON.h"
#include "Application.h"

#include "MathGeoLib/include/MathGeoLib.h"

#include <vector>

GameObject::GameObject() : enabled(true), name("Empty Game Object"), _parent(nullptr), to_delete(false), transform(nullptr), _visible(false)
{
	transform = (Transform*)AddComponent(TRANSFORM);
	UUID = LCG().Int();
}

GameObject::GameObject(ComponentType component) : GameObject()
{
	AddComponent(component);

	switch (component)
	{
	case ComponentType::CAMERA:
		name = "Camera";
		break;
	case ComponentType::LIGHT:
		name = "Light";
		break;
	default:
		break;
	}
}

GameObject::~GameObject()
{
	_parent = nullptr;

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
			//Update Components
			if (components[i]->IsEnabled()) 
			{
				if (components[i]->GetType() == ComponentType::MESH) 
				{
					GnMesh* mesh = (GnMesh*)components[i];
					GenerateAABB(mesh);

					if(App->renderer3D->IsInsideCameraView(_AABB))
						mesh->Update();
				}
				else
				{
					components[i]->Update();
				}
			}
		}

		//Update Children
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

	//if (ImGui::InputText("##file_selector", selected_file, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	char* buf = (char*)name.c_str();
	ImGui::InputText("##name", buf, 64, ImGuiInputTextFlags_EnterReturnsTrue);

	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->OnEditor();
	}

	if(ImGui::CollapsingHeader("Debugging Information")) 
	{
		if(_parent != nullptr)
			ImGui::Text("Parent: %s", _parent->GetName());
		else 
			ImGui::Text("No parent");

		ImGui::Text("UUID: %d", UUID);
	}
}

void GameObject::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("UUID", UUID);

	if(_parent != nullptr)
		save_object.AddInt("Parent UUID",_parent->UUID);
	else 
		save_object.AddInt("Parent UUID", 0);

	save_object.AddString("Name", name.c_str());

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
	name = object->GetString("Name", "No Name");
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

uint GameObject::LoadNodeData(GnJSONObj* object)
{
	UUID = object->GetInt("UUID");
	name = object->GetString("Name", "No Name");
	uint parentUUID = object->GetInt("Parent UUID");

	float3 position = object->GetFloat3("Position");
	transform->SetPosition(position);

	Quat rotation = object->GetQuaternion("Rotation");
	transform->SetRotation(rotation);

	float3 scale = object->GetFloat3("Scale");
	transform->SetScale(scale);

	int meshID = object->GetInt("Mesh", -1);
	if (meshID != -1) {
		GnMesh* mesh = (GnMesh*)AddComponent(ComponentType::MESH);
		mesh->SetResourceUID(meshID);
	}

	int materialID = object->GetInt("Material", -1);
	if (materialID != -1) {
		Material* material = (Material*)AddComponent(ComponentType::MATERIAL);
		material->SetResourceUID(materialID);
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

	switch (type)
	{
	case TRANSFORM:
		if (transform != nullptr)
		{
			RemoveComponent(transform);
		}

		transform = new Transform();
		component = transform;
		break;
	case MESH:
		component = new GnMesh();
		break;
	case MATERIAL:
		component = new Material(this);
		break;
	case CAMERA:
		component = new Camera(this);
		break;
	case LIGHT:
		component = new Light(this);
		break;
	default:
		break;
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

AABB GameObject::GetAABB()
{
	return _AABB;
}

bool GameObject::IsVisible()
{
	return _visible;
}

void GameObject::AddChild(GameObject* child)
{
	if(child != nullptr)
		children.push_back(child);

	child->SetParent(this);
}

int GameObject::GetChildrenAmount()
{
	return children.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return children[index];
}

GameObject* GameObject::GetParent()
{
	return _parent;
}

void GameObject::SetParent(GameObject* g_parent)
{
	_parent = g_parent;
}

void GameObject::Reparent(GameObject* newParent)
{
	if (newParent != nullptr) 
	{
		_parent->RemoveChild(this);
		_parent = newParent;
		newParent->AddChild(this);
		transform->ChangeParentTransform(newParent->GetTransform()->GetGlobalTransform());
	}
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
		children[i]->UpdateChildrenTransforms();
	}
}

void GameObject::GenerateAABB(GnMesh* mesh)
{
	if (mesh->GetResource(ResourceType::RESOURCE_MESH) == nullptr)
		return;

	_OBB = mesh->GetAABB();
	_OBB.Transform(transform->GetGlobalTransform());

	_AABB.SetNegativeInfinity();
	_AABB.Enclose(_OBB);

	float3 cornerPoints[8];
	_AABB.GetCornerPoints(cornerPoints);
	App->renderer3D->DrawAABB(cornerPoints);
}
