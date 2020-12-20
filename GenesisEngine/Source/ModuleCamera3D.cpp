#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "GnJSON.h"
#include "Camera.h"

#include "Mesh.h"
#include "ResourceMesh.h"
#include "GameObject.h"
#include "Transform.h"


ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "camera";

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	_position = float3(0.0f, 0.75f, 3.5f);
	_reference = float3(0.0f, 0.75f, 0.0f);

	_camera = new Camera();
	_camera->SetPosition(float3(_position));
	_camera->SetReference(_reference);
	LookAt(_reference);

	background = { 0.12f, 0.12f, 0.12f, 1.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init() {return true; }

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::LoadConfig(GnJSONObj& config)
{
	move_speed = config.GetFloat("move_speed");
	drag_speed = config.GetFloat("drag_speed");
	orbit_speed = config.GetFloat("orbit_speed");
	zoom_speed = config.GetFloat("zoom_speed");
	sensitivity = config.GetFloat("sensitivity");

	return true;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete _camera;
	_camera = nullptr;

	return true;
}

void ModuleCamera3D::OnResize(int width, int height)
{
	_camera->AdjustFieldOfView(width, height);
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (!App->editor->IsSceneFocused())
		return UPDATE_CONTINUE;

	float3 newPos = float3::zero;
	int speed_multiplier = 1;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed_multiplier = 2;

	//Up/Down
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_REPEAT) 
		newPos.y += move_speed * dt;
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) 
		newPos.y -= move_speed * dt;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
	{
		if (App->scene->selectedGameObject != nullptr)
		{
			float3 center(App->scene->selectedGameObject->GetTransform()->GetPosition());
			LookAt(center);
		}
	}

	//Forwards/Backwards
	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)) 
		newPos += _camera->GetFrustum().front * move_speed * speed_multiplier * dt;
	if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)) 
		newPos -= _camera->GetFrustum().front * move_speed * speed_multiplier * dt;

	//Left/Right
	if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)) 
		newPos += _camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * dt;
	if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT))
		newPos -= _camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * dt;

	//Drag
	if ((App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT))
	{
		newPos -= _camera->GetFrustum().WorldRight() * App->input->GetMouseXMotion() * drag_speed * dt;
		newPos += _camera->GetFrustum().up * App->input->GetMouseYMotion() * drag_speed * dt;
	}

	// Zoom 
	if (App->input->GetMouseZ() > 0)
		newPos += _camera->GetFrustum().front * zoom_speed * dt;
	else if(App->input->GetMouseZ() < 0 )
		newPos -= _camera->GetFrustum().front * zoom_speed * dt;

	if ((App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)||
		((App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) && (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)))
		Orbit(dt);

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsOver() && App->editor->MouseOnScene())
	{
		GameObject* pickedGameObject = PickGameObject();
		App->scene->selectedGameObject = pickedGameObject;
	}

	_position += newPos;
	_camera->SetPosition(_position);
	_reference += newPos;
	_camera->SetReference(_reference);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	_camera->LookAt(Spot);
	_reference = Spot;
}

Camera* ModuleCamera3D::GetCamera() { return _camera; }

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	_position += Movement;
	_reference += Movement;
}

void ModuleCamera3D::Orbit(float dt)
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Quat y_rotation(_camera->GetFrustum().up, dx * dt * orbit_speed * 0.1f);
	Quat x_rotation(_camera->GetFrustum().WorldRight(), dy * dt * orbit_speed * 0.1f);

	float3 distance = _position - _reference;
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	_position = distance + _reference;
	_camera->SetPosition(_position);
	_camera->LookAt(_reference);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return _camera->GetViewMatrix();
}

float4x4 ModuleCamera3D::GetViewMatrixM()
{
	return _camera->GetFrustum().ViewMatrix();
}

float* ModuleCamera3D::GetProjectionMatrix()
{
	return _camera->GetProjectionMatrix();
}

float4x4 ModuleCamera3D::GetProjectionMatrixM()
{
	return _camera->GetFrustum().ProjectionMatrix();
}

float3 ModuleCamera3D::GetPosition()
{
	return _position;
}

GameObject* ModuleCamera3D::PickGameObject()
{
	float normalized_x =  App->editor->mouseScenePosition.x / App->editor->image_size.x;
	float normalized_y =  App->editor->mouseScenePosition.y / App->editor->image_size.y;

	normalized_x =   (normalized_x - 0.5f) * 2.0f;
	normalized_y =  -(normalized_y - 0.5f) * 2.0f;

	LineSegment my_ray = _camera->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

	App->renderer3D->_ray = my_ray;

	std::vector<GameObject*> sceneGameObjects = App->scene->GetAllGameObjects();
	std::map<float, GameObject*> hitGameObjects;

	//find all hit GameObjects
	for (size_t i = 0; i < sceneGameObjects.size(); i++) 
	{
		bool hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB());

		if (hit)
		{
			float dNear;
			float dFar;
			hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB(), dNear, dFar);
			hitGameObjects[dNear] = sceneGameObjects[i];
		}
	}

	std::map<float, GameObject*>::iterator it = hitGameObjects.begin();
	for (it; it != hitGameObjects.end() ; it++)
	{
		GameObject* gameObject = it->second;

		LineSegment ray_local_space = my_ray;
		ray_local_space.Transform(gameObject->GetTransform()->GetGlobalTransform().Inverted());

		GnMesh* mesh = (GnMesh*)gameObject->GetComponent(ComponentType::MESH);

		if (mesh == nullptr)
			continue;

		ResourceMesh* resourceMesh = dynamic_cast<ResourceMesh*>(mesh->GetResource(ResourceType::RESOURCE_MESH));

		if (resourceMesh == nullptr)
			continue;

		for (size_t i = 0; i < resourceMesh->indices_amount; i+=3)
		{
			//create every triangle
			float3 v1(resourceMesh->vertices[resourceMesh->indices[i] * 3], resourceMesh->vertices[resourceMesh->indices[i] * 3 + 1],
				      resourceMesh->vertices[resourceMesh->indices[i] * 3 + 2]);

			float3 v2(resourceMesh->vertices[resourceMesh->indices[i+1] * 3], resourceMesh->vertices[resourceMesh->indices[i+1] * 3 + 1],
				      resourceMesh->vertices[resourceMesh->indices[i+1] * 3 + 2]);

			float3 v3(resourceMesh->vertices[resourceMesh->indices[i+2] * 3], resourceMesh->vertices[resourceMesh->indices[i+2] * 3 + 1],
				      resourceMesh->vertices[resourceMesh->indices[i+2] * 3 + 2]);

			const Triangle triangle(v1, v2, v3);

			float distance;
			float3 intersectionPoint;
			if (ray_local_space.Intersects(triangle, &distance, &intersectionPoint)) {
				return gameObject;
			}
		}
	}

	return nullptr;
}

FixedFOV ModuleCamera3D::GetFixedFOV()
{
	return _camera->fixedFOV;
}

void ModuleCamera3D::SetFixedFOV(FixedFOV fixedFOV)
{
	_camera->SetFixedFOV(fixedFOV);
}

float ModuleCamera3D::GetVerticalFieldOfView()
{
	return _camera->GetFrustum().verticalFov;
}

float ModuleCamera3D::GetHorizontalFieldOfView()
{
	return _camera->GetFrustum().horizontalFov;
}


void ModuleCamera3D::SetVerticalFieldOfView(float verticalFOV, int screen_width, int screen_height)
{
	_camera->SetVerticalFieldOfView(verticalFOV, screen_width, screen_height);
	App->renderer3D->UpdateProjectionMatrix(_camera->GetProjectionMatrix());
}

void ModuleCamera3D::SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height)
{
	_camera->SetHorizontalFieldOfView(horizontalFOV, screen_width, screen_height);
	App->renderer3D->UpdateProjectionMatrix(_camera->GetProjectionMatrix());
}

void ModuleCamera3D::Reset()
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	_position = float3(0.0f, 0.0f, 5.0f);
	_reference = float3(0.0f, 0.0f, 0.0f);

	Move(float3(1.0f, 1.0f, 0.0f));
	LookAt(float3(0.0f, 0.0f, 0.0f));
}

void ModuleCamera3D::SetBackgroundColor(float r, float g, float b, float w)
{
	background = { r,g,b,w };
}

