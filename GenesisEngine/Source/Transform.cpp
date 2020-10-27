#include "Transform.h"
#include "ImGui/imgui.h"
#include "Globals.h"

Transform::Transform() : Component()
{
	type = ComponentType::TRANSFORM;

	position.x = position.y = position.z = 0.0f;
	rotation = Quat::identity;
	scale.x = scale.y = scale.z = 1.0f;

	transform.SetIdentity();
	globalTransform.SetIdentity();

	globalTransform = float4x4::FromTRS(position, rotation, scale);

	//transform.SetIdentity();
	//globalTransform = transform;
}

Transform::Transform(float3 g_position, Quat g_rotation, float3 g_scale) : Component()
{
	position = g_position;
	rotation = g_rotation;
	scale = g_scale;

	transform.SetIdentity();
	globalTransform.SetIdentity();

	transform = float4x4::FromTRS(g_position, g_rotation, g_scale);
	globalTransform = transform;
}

Transform::~Transform() {}

void Transform::Update() {}

void Transform::OnEditor()
{
	if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float position4f[4] = { position.x, position.y, position.z, 1.0f };
		if (ImGui::DragFloat3("Position", position4f, 0.1f, -10000.0f, 10000.0f))
		{
			position.x = position4f[0];
			position.y = position4f[1];
			position.z = position4f[2];

			SetPosition(position);
		}

		float rotation4f[4] = { rotation.x, rotation.y, rotation.z, 1.0f };
		if (ImGui::DragFloat3("Rotation", rotation4f, 0.1f, -360.0f, 360.0f)) 
		{
			rotation.x = rotation4f[0];
			rotation.y = rotation4f[1];
			rotation.z = rotation4f[2];

			SetRotation(rotation);
		}

		float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
		if (ImGui::DragFloat3("Scale", scale4f, 0.01f, -10000.0f, 10000.0f)) 
		{
			scale.x = scale4f[0];
			scale.y = scale4f[1];
			scale.z = scale4f[2];

			SetScale(scale);
		};

		ImGui::Spacing();
	}
}

float4x4 Transform::GetTransform()
{
	return transform;
}

float4x4 Transform::GetGlobalTransform()
{
	return globalTransform;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	globalTransform.SetTranslatePart(position);
}

void Transform::SetPosition(float3 new_position)
{
	position = new_position;

	globalTransform.SetTranslatePart(position);
}

float3 Transform::GetPosition()
{
	return position;
}

void Transform::SetRotation(float x, float y, float z)
{
	rotation = Quat::FromEulerXYZ(x, y, z);

	globalTransform.SetRotatePart(rotation);
}

void Transform::SetRotation(Quat new_rotation)
{
	rotation = new_rotation;

	globalTransform.SetRotatePart(rotation);
}

void Transform::SetRotation(float x, float y, float z, float w)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	rotation.w = w;

	globalTransform.SetRotatePart(rotation);
}

Quat Transform::GetRotation()
{
	return rotation;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	globalTransform.Scale(scale);
}

void Transform::SetScale(float3 new_scale)
{
	scale = new_scale;

	globalTransform.Scale(scale);
}

float3 Transform::GetScale()
{
	return scale;
}

