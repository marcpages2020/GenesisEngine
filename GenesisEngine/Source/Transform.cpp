#include "Transform.h"
#include "ImGui/imgui.h"
#include "Globals.h"

Transform::Transform() : Component()
{
	type = ComponentType::TRANSFORM;

	position.x = position.y = position.z = 0.0f;
	rotation.x = rotation.y = rotation.z = 0.0f;
	scale.x = scale.y = scale.z = 1.0f;
}

Transform::Transform(float3 g_position, Quat g_rotation, float3 g_scale)
{
	position = g_position;
	rotation = g_rotation;
	scale = g_scale;
}

Transform::~Transform() {}

void Transform::Update() {}

void Transform::OnEditor()
{
	if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float position4f[4] = { position.x, position.y, position.z, 1.0f };
		if (ImGui::DragFloat3("Position", position4f, 0.1f, -10000.0f, 10000.0f)) {
			position.x = position4f[0];
			position.y = position4f[1];
			position.z = position4f[2];
		}

		float rotation4f[4] = { rotation.x, rotation.y, rotation.z, 1.0f };
		if (ImGui::DragFloat3("Rotation", rotation4f, 0.1f, -360.0f, 360.0f)) 
		{
			rotation.x = rotation4f[0];
			rotation.y = rotation4f[1];
			rotation.z = rotation4f[2];
		}

		float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
		if (ImGui::DragFloat3("Scale", scale4f, 0.01f, -10000.0f, 10000.0f)) 
		{
			scale.x = scale4f[0];
			scale.y = scale4f[1];
			scale.z = scale4f[2];
		};

		ImGui::Spacing();
	}
}

