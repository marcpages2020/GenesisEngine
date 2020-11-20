#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"

Camera::Camera() : Component(nullptr), _aspectRatio(AspectRatio::AR_16_9), fixedFOV(FIXED_HORIZONTAL_FOV) {
	type = ComponentType::CAMERA;

	_frustum.type = FrustumType::PerspectiveFrustum;
	
	_frustum.pos = float3(0.0f, 0.0f, -5.0f);
	_frustum.up = float3(0.0f, 1.0f, 0.0f);
	_frustum.front = float3(0.0f, 0.0f, 1.0f);

	_frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	_frustum.nearPlaneDistance = 0.3f;
	_frustum.farPlaneDistance = 100.0f;
}

Camera::Camera(GameObject* gameObject) : Component(gameObject), _aspectRatio(AspectRatio::AR_16_9)
{
	type = ComponentType::CAMERA;

	_frustum.type = FrustumType::PerspectiveFrustum;

	_frustum.pos = float3(0.0f, 0.0f, -5.0f);
	_frustum.up = float3(0.0f, 1.0f, 0.0f);
	_frustum.front = float3(0.0f, 0.0f, 1.0f);

	_frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	_frustum.nearPlaneDistance = 0.3f;
	_frustum.farPlaneDistance = 100.0f;
}

Camera::~Camera(){}

void Camera::Update()
{
	_frustum.pos = _gameObject->GetTransform()->GetPosition();
	
	_frustum.up = _gameObject->GetTransform()->GetGlobalTransform().WorldY();
	_frustum.front = _gameObject->GetTransform()->GetGlobalTransform().WorldZ();

	float3 corner_points[8];
	_frustum.GetCornerPoints(corner_points);
	App->renderer3D->DrawAABB(corner_points);
}

void Camera::OnEditor()
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float horizontalFOV = _frustum.horizontalFov * RADTODEG;
		if (ImGui::DragFloat("Horizontal FOV", &horizontalFOV, 0.02f, 0.0f, 130.0f)) {
			_frustum.horizontalFov = horizontalFOV * DEGTORAD;
			AdjustFieldOfView();
		}

		float verticalFOV = _frustum.verticalFov * RADTODEG;
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV, 0.02f, 0.0f, 60.0f)) {
			_frustum.verticalFov = verticalFOV * DEGTORAD;
			AdjustFieldOfView();
		}

		ImGui::Spacing();
	}
}

void Camera::AdjustFieldOfView()
{
	switch (_aspectRatio)
	{
	case AR_16_9:
		//_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (16.0f / 9.0f));
		_frustum.verticalFov = 2 * atan(tan(_frustum.horizontalFov * 0.5f) * (9.0f / 16.0f));
		break;
	case AR_16_10:
		//_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (16.0f / 10.0f));
		_frustum.verticalFov = 2 * atan(tan(_frustum.horizontalFov * 0.5f) * (10.0f / 16.0f));
		break;
	case AR_4_3:
		//_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (4.0f / 3.0f));
		_frustum.verticalFov = 2 * atan(tan(_frustum.horizontalFov * 0.5f) * (3.0f / 4.0f));
		break;
	default:
		break;
	}
}

void Camera::AdjustFieldOfView(float width, float height)
{
	if(fixedFOV == FIXED_HORIZONTAL_FOV)
		_frustum.verticalFov = 2.0f * std::atan(std::tan(_frustum.horizontalFov * 0.5f) * (height / width));
	else 
		_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (width / height));
}

void Camera::SetVerticalFieldOfView(float verticalFOV, float screen_width, float screen_height)
{
	_frustum.verticalFov = verticalFOV;
	_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (screen_width / screen_height));
}

void Camera::SetHorizontalFieldOfView(float horizontalFOV, float screen_width, float screen_height)
{
	_frustum.horizontalFov = horizontalFOV;
	_frustum.verticalFov = 2.0f * std::atan(std::tan(_frustum.horizontalFov * 0.5f) * (screen_height / screen_width));
}

void Camera::SetPosition(float3 position)
{
	_frustum.pos = position;
}

void Camera::SetReference(float3 reference)
{
	_reference = reference;
}

void Camera::Look(float3 spot)
{
	float3 difference = spot - _frustum.pos;

	float3x3 matrix = float3x3::LookAt(_frustum.front, difference.Normalized(), _frustum.up, float3::unitY);

	_frustum.front = matrix.MulDir(_frustum.front).Normalized();
	_frustum.up = matrix.MulDir(_frustum.up).Normalized();
}

Frustum Camera::GetFrustum()
{
	return _frustum;
}

float* Camera::GetViewMatrix()
{
	float4x4 viewMatrix;

	viewMatrix = _frustum.ViewMatrix();
	viewMatrix.Transpose();

	return (float*)viewMatrix.v;
}

float* Camera::GetProjectionMatrix()
{
	float4x4 projectionMatrix;

	projectionMatrix = _frustum.ProjectionMatrix();
	projectionMatrix.Transpose();

	return (float*)projectionMatrix.v;
}

bool Camera::ContainsAABB(AABB& aabb)
{
	float3 cornerPoints[8];
	int totalInside = 0;

	//get frustum planes
	Plane* m_plane = new Plane[6](); 
	_frustum.GetPlanes(m_plane);

	//get AABB points
	aabb.GetCornerPoints(cornerPoints); 

	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;

		//test all planes
		for (int i = 0; i < 8; ++i) {
			if (m_plane[p].IsOnPositiveSide(cornerPoints[i]) == true) { 
				iPtIn = 0;
				--iInCount;
			}
		}
		
		if(iInCount == 0)
			return false;

		totalInside += iPtIn;
	}

	//Totally inside camera view
	if (totalInside == 6)
		return true;

	//Partly inside camera view
	return true;
}
