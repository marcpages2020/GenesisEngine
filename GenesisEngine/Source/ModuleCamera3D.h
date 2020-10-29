#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init();
	bool Start();
	bool LoadConfig(JSON_Object* config) override;
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot);
	void Move(const vec3& Movement);
	float* GetViewMatrix();

	void Reset();
	void SetBackgroundColor(float r, float g, float b, float w);

private:

	void CalculateViewMatrix();

public:

	vec3 X, Y, Z, Position, Reference;
	Color background;

	float move_speed;
	float drag_speed;
	float zoom_speed;
	float sensitivity;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};