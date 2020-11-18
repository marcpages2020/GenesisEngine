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
	bool LoadConfig(GnJSONObj& config) override;
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	float* GetViewMatrix();

	void Reset();
	void SetBackgroundColor(float r, float g, float b, float w);

private:

	void CalculateViewMatrix();

public:

	float3 X, Y, Z;
	Color background;

	float move_speed;
	float drag_speed;
	float zoom_speed;
	float sensitivity;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;

	Camera* _camera;
	float3 _position;
	float3 _reference;
};