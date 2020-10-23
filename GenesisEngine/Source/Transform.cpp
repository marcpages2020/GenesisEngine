#include "Transform.h"

Transform::Transform() : Component()
{
	type = ComponentType::TRANSFORM;
}

Transform::Transform(float3 g_position, Quat g_rotation, float3 g_scale)
{
	position = g_position;
	rotation = g_rotation;
	scale = g_scale;
}

Transform::~Transform() {}

void Transform::Update() {}

