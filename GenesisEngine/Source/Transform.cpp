#include "Transform.h"
#include "ImGui/imgui.h"
#include "Globals.h"
#include "GameObject.h"

Transform::Transform() : Component()
{
	type = ComponentType::TRANSFORM;

	position.x = position.y = position.z = 0.0f;
	rotation = Quat::identity;
	eulerRotation = float3::zero;
	scale.x = scale.y = scale.z = 1.0f;

	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = localTransform;
}

Transform::Transform(float3 g_position, Quat g_rotation, float3 g_scale) : Component()
{
	position = g_position;
	rotation = g_rotation;
	eulerRotation = rotation.ToEulerXYZ();
	eulerRotation *= RADTODEG;
	scale = g_scale;

	localTransform = float4x4::FromTRS(g_position, g_rotation, g_scale);
	globalTransform = localTransform;
}

Transform::~Transform() {}

void Transform::Update() {}

void Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float position4f[4] = { position.x, position.y, position.z, 1.0f };
		if (ImGui::DragFloat3("Position", position4f, 0.1f, -10000.0f, 10000.0f))
		{
			position.x = position4f[0];
			position.y = position4f[1];
			position.z = position4f[2];

			SetPosition(position);
			UpdateLocalTransform();
		}

		float rotation4f[4] = { eulerRotation.x, eulerRotation.y, eulerRotation.z, 1.0f };
		if (ImGui::DragFloat3("Rotation", rotation4f, 0.1f, -360.0f, 360.0f))
		{
			eulerRotation.x = rotation4f[0];
			eulerRotation.y = rotation4f[1];
			eulerRotation.z = rotation4f[2];

			SetRotation(eulerRotation.x, eulerRotation.y, eulerRotation.z);
			UpdateLocalTransform();
		}

		float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
		if (ImGui::DragFloat3("Scale", scale4f, 0.01f, -10000.0f, 10000.0f))
		{
			scale.x = scale4f[0];
			scale.y = scale4f[1];
			scale.z = scale4f[2];

			SetScale(scale);
			UpdateLocalTransform();
		};

		float scale_multiplier = scale.x;
		if (ImGui::DragFloat("Scale Multiplier", &scale_multiplier, 0.01f, 0.0f, 10.0f)) 
		{
			SetProportionalScale(scale_multiplier);
		}

		ImGui::Spacing();

		if (ImGui::Button("Reset"))
			Reset();

		ImGui::Spacing();
	}
}

void Transform::Set(float4x4 g_transform)
{
	localTransform = g_transform;
}

float4x4 Transform::GetLocalTransform()
{
	return localTransform;
}

float4x4 Transform::GetGlobalTransform()
{
	return globalTransform;
}

void Transform::UpdateLocalTransform()
{
	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = localTransform;

	/*if (gameObject->GetParent() == nullptr)
		globalTransform = localTransform;
	else
	{
		float4x4 parentGlobal = gameObject->GetParent()->GetTransform()->GetGlobalTransform();
		UpdateGlobalTransform(parentGlobal);
	}*/

}

void Transform::UpdateGlobalTransform(float4x4 parentGlobalTransform)
{
	localTransform = float4x4::FromTRS(position, rotation, scale);
	//globalTransform = localTransform;
	globalTransform = parentGlobalTransform * localTransform;
}

void Transform::Reset()
{
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
	eulerRotation = float3::zero;

	localTransform = float4x4::FromTRS(position, rotation, scale);
	globalTransform = localTransform;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	//UpdateLocalTransform();
}

void Transform::SetPosition(float3 new_position)
{
	position = new_position;

	//UpdateLocalTransform();
}

float3 Transform::GetPosition()
{
	return position;
}

//Set rotation from Euler angles
void Transform::SetRotation(float x, float y, float z)
{
	eulerRotation = float3(x, y, z);
	rotation = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
	//UpdateLocalTransform();
}

void Transform::SetRotation(Quat new_rotation)
{
	rotation = new_rotation;
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	//UpdateLocalTransform();
}

void Transform::SetRotation(float x, float y, float z, float w)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
	rotation.w = w;

	//UpdateLocalTransform();
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

	//UpdateLocalTransform();
}

void Transform::SetScale(float3 new_scale)
{
	scale = new_scale;

	//UpdateLocalTransform();
}

void Transform::SetProportionalScale(float multiplier)
{
	scale.x = scale.y = scale.z = multiplier;

	UpdateLocalTransform();
}

float3 Transform::GetScale()
{
	return scale;
}

