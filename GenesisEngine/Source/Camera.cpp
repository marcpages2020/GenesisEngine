#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"

Camera::Camera(GameObject* gameObject) : Component(gameObject) 
{
	type = ComponentType::CAMERA;
	_gameObject = gameObject;

	_frustum.type = FrustumType::PerspectiveFrustum;

	_frustum.pos = float3(0.0f, 0.0f, 0.0f);
	_frustum.up = float3(0.0f, 1.0f, 0.0f);
	_frustum.front = float3(0.0f, 0.0f, 1.0f);

	_frustum.horizontalFov = 40.0f;
	_frustum.verticalFov = 30.0f;

	_frustum.nearPlaneDistance = 0.3f;
	_frustum.farPlaneDistance = 20.0f;
}

Camera::~Camera(){}

void Camera::Update()
{
	_frustum.pos = _gameObject->GetTransform()->GetPosition();
	//_frustum.ProjectionMatrix().;
	LOG("Yee");
	float3 corner_points[8];
	_frustum.GetCornerPoints(corner_points);
	App->renderer3D->DrawAABB(corner_points);
}

void Camera::OnEditor()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat("Horizontal FOV", &_frustum.horizontalFov, 0.01f, 0.0f, 130.0f));
		if (ImGui::DragFloat("Vertical FOV", &_frustum.verticalFov, 0.01f, 0.0f, 60.0f));

		ImGui::Spacing();
	}
}
