#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"
#include "Transform.h"
#include "GnJSON.h"
#include "Camera.h"

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	name = "camera";

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	_position = float3(5.0f, 5.0f, 5.0f);
	_reference = float3(0.0f, 0.0f, 0.0f);

	_camera = new Camera();
	_camera->SetPosition(float3(_position));
	_camera->SetReference(_reference);
	Look(_reference);

	background = { 0.12f, 0.12f, 0.12f, 1.0f };
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Init()
{
	return true;
}

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
			float3 center(App->scene->selectedGameObject->GetTransform()->GetPosition().x,
				          App->scene->selectedGameObject->GetTransform()->GetPosition().y,
				          App->scene->selectedGameObject->GetTransform()->GetPosition().z);

			LookAt(center);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		Reset();

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

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) 
	{
		GameObject* pickedGameObject = PickGameObject();
		App->scene->selectedGameObject = pickedGameObject;
	}

	_position += newPos;
	_camera->SetPosition(_position);
	_reference += newPos;
	_camera->SetReference(_reference);

	float3 corner_points[8];
	_camera->GetFrustum().GetCornerPoints(corner_points);
	App->renderer3D->DrawAABB(corner_points);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(float3& position)
{
	_camera->Look(position);
	_reference = position;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& Spot)
{
	_camera->Look(Spot);
}


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
	_camera->Look(_reference);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return _camera->GetViewMatrix();
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
	LOG("Picking x: %.2f y: %.2f", normalized_x, normalized_y);

	App->renderer3D->_ray = my_ray;

	std::vector<GameObject*> sceneGameObjects = App->scene->GetAllGameObjects();
	float closest_distance;
	float closest_hit_point;

	for (size_t i = 0; i < sceneGameObjects.size(); i++)
	{
		bool hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB());

		if (hit)
		{
			float distance;
			float hit_point;			

			hit = my_ray.Intersects(sceneGameObjects[i]->GetAABB(), distance, hit_point);
			LOG("Intersection");
			return sceneGameObjects[i];
		}
	}

	return nullptr;
}

float ModuleCamera3D::GetVerticalFieldOfView()
{
	return _camera->GetFrustum().verticalFov;
}

float ModuleCamera3D::GetHorizontalFieldOfView()
{
	return _camera->GetFrustum().horizontalFov;
}


void ModuleCamera3D::SetFieldOfView(float verticalFOV, int screen_width, int screen_height)
{
	_camera->SetFieldOfView(verticalFOV, screen_width, screen_height);
	
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

