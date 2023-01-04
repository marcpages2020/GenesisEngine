#include "Globals.h"
#include "Engine.h"
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

	_camera = new Camera();
	SetPosition(float3(60.5f, 40.5f, 90.5f));
	LookAt(float3(0.0f, 0.0f, 0.0f));

	background = { 0.12f, 0.12f, 0.12f, 1.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init() { return true; }

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
update_status ModuleCamera3D::Update(float deltaTime)
{
	if (!engine->editor->IsWindowFocused(WindowType::WINDOW_SCENE))
		return UPDATE_CONTINUE;

	float3 newPos = float3::zero;
	int speed_multiplier = 1;

	if (engine->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed_multiplier = 2;

	//Up/Down
	if (engine->input->GetKey(SDL_SCANCODE_O) == KEY_REPEAT)
		newPos.y += move_speed * deltaTime;
	if (engine->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
		newPos.y -= move_speed * deltaTime;

	if (engine->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (engine->scene->selectedGameObject != nullptr)
		{
			FocusOnGameObject(engine->scene->selectedGameObject);
			//LookAt(engine->scene->selectedGameObject->GetTransform()->GetPosition());
		}
	}

	//Forwards/Backwards
	if ((engine->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (engine->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT))
		newPos += _camera->GetFrustum().front * move_speed * speed_multiplier * deltaTime;
	if ((engine->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) || (engine->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT))
		newPos -= _camera->GetFrustum().front * move_speed * speed_multiplier * deltaTime;

	//Left/Right
	if ((engine->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) || (engine->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT))
		newPos += _camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * deltaTime;
	if ((engine->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) || (engine->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT))
		newPos -= _camera->GetFrustum().WorldRight() * move_speed * speed_multiplier * deltaTime;

	//Drag
	if ((engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT))
	{
		newPos -= _camera->GetFrustum().WorldRight() * engine->input->GetMouseXMotion() * drag_speed * deltaTime;
		newPos += _camera->GetFrustum().up * engine->input->GetMouseYMotion() * drag_speed * deltaTime;
	}

	// Zoom 
	if (engine->input->GetMouseZ() > 0)
		newPos += _camera->GetFrustum().front * zoom_speed * deltaTime;
	else if (engine->input->GetMouseZ() < 0)
		newPos -= _camera->GetFrustum().front * zoom_speed * deltaTime;

	if (engine->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		if (engine->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			newPos += _camera->GetFrustum().front * zoom_speed * 2.0 * engine->input->GetMouseXMotion() * deltaTime;
		}

		if ((engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN) && engine->scene->selectedGameObject) {
			LookAt(engine->scene->selectedGameObject->GetTransform()->GetPosition());
		}

		if (engine->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT) {
			Orbit(deltaTime);
		}
	}

	if (engine->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && !ImGuizmo::IsUsing() && engine->editor->MouseOnScene())
	{
		GameObject* pickedGameObject = PickGameObject();
		engine->scene->selectedGameObject = pickedGameObject;
	}

	SetPosition(_position += newPos);
	SetReference(GetReference() += newPos);

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::SetPosition(float3 newPosition)
{
	_position = newPosition;
	_camera->SetPosition(newPosition);
}

void ModuleCamera3D::SetReference(float3 newReference)
{
	_camera->SetReference(newReference);
}

float3 ModuleCamera3D::GetReference()
{
	return _camera->GetReference();
}

void ModuleCamera3D::FocusOnGameObject(GameObject* gameObjectToFocus)
{
	if (!gameObjectToFocus) { return; }

	float3 objectPosition = engine->scene->selectedGameObject->GetTransform()->GetPosition();

	float objectExtension = 0.0f;
	gameObjectToFocus->GetMaxExtension(objectExtension);
	objectExtension = 30.0f;
	
	LookAt(objectPosition);

	float3 directionVector = (_position - objectPosition).Normalized();
	float3 newPosition = objectPosition + directionVector * objectExtension;
	//SetPosition(newPosition);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	_camera->LookAt(Spot);
	//_reference = Spot;
}

Camera* ModuleCamera3D::GetCamera() { return _camera; }

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	_position += Movement;
	_camera->SetReference(_camera->GetReference() += Movement);
	//_reference += Movement;
}

void ModuleCamera3D::Orbit(float deltaTime)
{
	int dx = -engine->input->GetMouseXMotion();
	int dy = engine->input->GetMouseYMotion();

	Quat y_rotation(_camera->GetUp(), dx * deltaTime * orbit_speed * 0.1f);
	Quat x_rotation(_camera->GetRight(), dy * deltaTime * orbit_speed * 0.1f);

	float3 distance = _position - _camera->GetReference();
	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	_position = distance + _camera->GetReference();
	_camera->SetPosition(_position);
	_camera->LookAt(GetReference());
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
	float normalized_x = engine->editor->mouseScenePosition.x / engine->editor->image_size.x;
	float normalized_y = engine->editor->mouseScenePosition.y / engine->editor->image_size.y;

	normalized_x = (normalized_x - 0.5f) * 2.0f;
	normalized_y = -(normalized_y - 0.5f) * 2.0f;

	LineSegment my_ray = _camera->GetFrustum().UnProjectLineSegment(normalized_x, normalized_y);

	engine->renderer3D->_ray = my_ray;

	std::vector<GameObject*> sceneGameObjects = engine->scene->GetAllGameObjects();
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
	for (it; it != hitGameObjects.end(); it++)
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

		for (size_t i = 0; i < resourceMesh->indices_amount; i += 3)
		{
			//create every triangle
			float3 v1(resourceMesh->vertices[resourceMesh->indices[i] * VERTEX_ATTRIBUTES], resourceMesh->vertices[resourceMesh->indices[i] * VERTEX_ATTRIBUTES + 1],
				resourceMesh->vertices[resourceMesh->indices[i] * VERTEX_ATTRIBUTES + 2]);

			float3 v2(resourceMesh->vertices[resourceMesh->indices[i + 1] * VERTEX_ATTRIBUTES], resourceMesh->vertices[resourceMesh->indices[i + 1] * VERTEX_ATTRIBUTES + 1],
				resourceMesh->vertices[resourceMesh->indices[i + 1] * VERTEX_ATTRIBUTES + 2]);

			float3 v3(resourceMesh->vertices[resourceMesh->indices[i + 2] * VERTEX_ATTRIBUTES], resourceMesh->vertices[resourceMesh->indices[i + 2] * VERTEX_ATTRIBUTES + 1],
				resourceMesh->vertices[resourceMesh->indices[i + 2] * VERTEX_ATTRIBUTES + 2]);

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
	engine->renderer3D->UpdateProjectionMatrix(_camera->GetProjectionMatrix());
}

void ModuleCamera3D::SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height)
{
	_camera->SetHorizontalFieldOfView(horizontalFOV, screen_width, screen_height);
	engine->renderer3D->UpdateProjectionMatrix(_camera->GetProjectionMatrix());
}

void ModuleCamera3D::Reset()
{
	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	SetPosition(float3(0.0f, 0.0f, 5.0f));
	SetReference(float3(0.0f, 0.0f, 0.0f));

	Move(float3(1.0f, 1.0f, 0.0f));
	LookAt(float3(0.0f, 0.0f, 0.0f));
}

void ModuleCamera3D::SetBackgroundColor(float r, float g, float b, float w)
{
	background = { r,g,b,w };
}

