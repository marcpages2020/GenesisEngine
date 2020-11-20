#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Camera;
class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	bool LoadConfig(GnJSONObj& config) override;

	void Look(float3& position);
	void LookAt(const float3& Spot);
	float* GetViewMatrix();
	float* GetProjectionMatrix();
	float3 GetPosition();
	GameObject* PickGameObject();
	float GetVerticalFieldOfView();
	float GetHorizontalFieldOfView();
	void SetVerticalFieldOfView(float verticalFOV, int screen_width, int screen_height);
	void SetHorizontalFieldOfView(float horizontalFOV, int screen_width, int screen_height);

	void Reset();
	void SetBackgroundColor(float r, float g, float b, float w);

private:
	void Move(const float3& Movement);
	void Orbit(float dt);

public:

	float3 X, Y, Z;
	Color background;

	float move_speed;
	float drag_speed;
	float orbit_speed;
	float zoom_speed;
	float sensitivity;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;

	Camera* _camera;
	float3 _position;
	float3 _reference;
};