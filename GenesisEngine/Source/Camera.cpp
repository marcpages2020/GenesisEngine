#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "GnJSON.h"

Camera::Camera() : Component(nullptr), _aspectRatio(16.0f/9.0f), fixedFOV(FIXED_HORIZONTAL_FOV) {
	type = ComponentType::CAMERA;

	_frustum.type = FrustumType::PerspectiveFrustum;
	
	_frustum.pos = float3(0.0f, 0.0f, -5.0f);
	_frustum.up = float3(0.0f, 1.0f, 0.0f);
	_frustum.front = float3(0.0f, 0.0f, 1.0f);

	_frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	_frustum.nearPlaneDistance = 0.3f;
	_frustum.farPlaneDistance = 1000.0f;
}

Camera::Camera(GameObject* gameObject) : Component(gameObject), _aspectRatio(16.0f/9.0f), fixedFOV(FIXED_HORIZONTAL_FOV)
{
	type = ComponentType::CAMERA;

	_frustum.type = FrustumType::PerspectiveFrustum;

	_frustum.pos = float3(0.0f, 0.0f, -5.0f);
	_frustum.up = float3(0.0f, 1.0f, 0.0f);
	_frustum.front = float3(0.0f, 0.0f, 1.0f);

	_frustum.horizontalFov = 60.0f * DEGTORAD;
	AdjustFieldOfView();

	_frustum.nearPlaneDistance = 0.3f;
	_frustum.farPlaneDistance = 1000.0f;
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
		ImGui::Spacing();

		bool fixedVerticalFOV = fixedFOV == FixedFOV::FIXED_VERTICAL_FOV;
		bool fixedHorizontalFOV = fixedFOV == FixedFOV::FIXED_HORIZONTAL_FOV;

		if (ImGui::RadioButton("Fixed VerticalFOV", fixedVerticalFOV))
			fixedFOV = FixedFOV::FIXED_VERTICAL_FOV;
		ImGui::SameLine();
		if (ImGui::RadioButton("Fixed HorizontalFOV", fixedHorizontalFOV))
			fixedFOV = FixedFOV::FIXED_HORIZONTAL_FOV;

		ImGui::Spacing();

		//Fixed Vertical FOV Settings
		if (fixedVerticalFOV)
		{
			float verticalFOV = _frustum.verticalFov * RADTODEG;
			if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
			{
				_frustum.verticalFov = verticalFOV * DEGTORAD;
				_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (_aspectRatio));
			}

			ImGui::Spacing();
			ImGui::Text("Horizontal FOV: %.2f", _frustum.horizontalFov * RADTODEG);
		}
		//Fixed Horizontal FOV Settings
		else
		{
			float horizontalFOV = _frustum.horizontalFov * RADTODEG;
			if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
			{
				_frustum.horizontalFov = horizontalFOV * DEGTORAD;
				_frustum.verticalFov = 2.0f * std::atan(std::tan(_frustum.horizontalFov * 0.5f) * (1 / _aspectRatio));
			}
			ImGui::Spacing();
			ImGui::Text("Vertical FOV: %.2f", _frustum.verticalFov * RADTODEG);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::DragFloat("Near Plane", &_frustum.nearPlaneDistance, 0.05f, 100.0f);
		ImGui::DragFloat("Far Plane", &_frustum.farPlaneDistance, 5.0f, 2000.0f);

		ImGui::Spacing();
	}
}

void Camera::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddString("name", name.c_str());
	save_object.AddInt("Type", type);
	bool mainCamera = App->renderer3D->GetMainCamera() == this;
	save_object.AddBool("Main Camera", mainCamera);
	save_object.AddFloat3("position", _frustum.pos);
	save_object.AddFloat3("up", _frustum.up);
	save_object.AddFloat3("front", _frustum.front);
	save_object.AddFloat("horizontalFOV", _frustum.horizontalFov * DEGTORAD);
	save_object.AddFloat("verticalFOV", _frustum.verticalFov * DEGTORAD);
	save_object.AddFloat("nearPlane", _frustum.nearPlaneDistance);
	save_object.AddFloat("farPlane", _frustum.farPlaneDistance);
	save_object.AddFloat("aspectRatio", _aspectRatio);
	save_object.AddFloat3("reference", _reference);

	save_array.AddObject(save_object);
}

void Camera::Load(GnJSONObj& load_object)
{
	if (load_object.GetBool("Main Camera", false))
		App->renderer3D->SetMainCamera(this);

	name = load_object.GetString("name", "camera");
	_frustum.pos = load_object.GetFloat3("position");
	_frustum.up = load_object.GetFloat3("up");
	_frustum.front = load_object.GetFloat3("front");
	_frustum.horizontalFov = load_object.GetFloat("horizontalFOV") * RADTODEG;
	_frustum.verticalFov = load_object.GetFloat("verticalFOV") * RADTODEG;
	_frustum.nearPlaneDistance = load_object.GetFloat("nearPlane");
	_frustum.farPlaneDistance = load_object.GetFloat("farPlane");
	_reference = load_object.GetFloat3("reference");
}

void Camera::SetFixedFOV(FixedFOV g_fixedFOV)
{
	fixedFOV = g_fixedFOV;
}

void Camera::AdjustFieldOfView()
{
	if(fixedFOV == FixedFOV::FIXED_HORIZONTAL_FOV)
		_frustum.verticalFov = 2 * atan(tan(_frustum.horizontalFov * 0.5f) * (1/_aspectRatio));
	else 
		_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (_aspectRatio));
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

float3 Camera::GetPosition()
{
	return _frustum.pos;
}

void Camera::SetReference(float3 reference)
{
	_reference = reference;
}

float3 Camera::GetReference()
{
	return _reference;
}

void Camera::SetNearPlaneDistance(float distance)
{
	_frustum.nearPlaneDistance = distance;
}

void Camera::SetFarPlaneDistance(float distance)
{
	_frustum.farPlaneDistance = distance;
}

void Camera::LookAt(float3 spot)
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
	Plane m_plane[6]; 
	_frustum.GetPlanes(m_plane);

	//get AABB points
	aabb.GetCornerPoints(cornerPoints); 

	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;

		for (int i = 0; i < 8; ++i) 
		{
			if (m_plane[p].normal.Dot(cornerPoints[i]) - m_plane[p].d >= 0.0f) 
			{
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

	//Partially inside camera view
	return true;
}
