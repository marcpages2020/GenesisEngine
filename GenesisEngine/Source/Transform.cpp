#include "Transform.h"
#include "ImGui/imgui.h"

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
		ImGui::DragFloat3("Position", position4f, 0.01f, 0.0f, 1.0f);

		float rotation4f[4] = { rotation.x, rotation.y, rotation.z, 1.0f };
		ImGui::DragFloat3("Rotation", rotation4f, 0.01f, 0.0f, 1.0f);

		float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
		ImGui::DragFloat3("Scale", scale4f, 0.01f, 0.0f, 1.0f);
	}
}

