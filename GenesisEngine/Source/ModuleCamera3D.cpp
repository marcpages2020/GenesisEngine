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
	_camera->Look(_reference);

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


	_position += newPos;
	_camera->SetPosition(_position);
	_reference += newPos;
	_camera->SetReference(_reference);

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(float3& position)
{
	_camera->Look(position);
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

	float3 distance = _position - _reference;

	Quat y_rotation(_camera->GetFrustum().up, dx * dt * orbit_speed * 0.1f);
	Quat x_rotation(_camera->GetFrustum().WorldRight(), dy * dt * orbit_speed * 0.1f);

	distance = x_rotation.Transform(distance);
	distance = y_rotation.Transform(distance);

	_position = distance + _reference;
	_camera->Look(_reference);
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	//return &ViewMatrix;
	return _camera->GetViewMatrix();;
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

