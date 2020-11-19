#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"

class Camera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool LoadConfig(GnJSONObj& config) override;

	void Look(float3& position);
	void LookAt(const float3& Spot);
	float* GetViewMatrix();
	float3 GetPosition();

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