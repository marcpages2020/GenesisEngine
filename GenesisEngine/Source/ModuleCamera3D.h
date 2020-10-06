#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot);
	void Move(const vec3& Movement);
	float* GetViewMatrix();

	void SetBackgroundColor(float r, float g, float b, float w);

private:

	void CalculateViewMatrix();

public:

	vec3 X, Y, Z, Position, Reference;
	Color background;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};