#pragma once
#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

enum FixedFOV {
	FIXED_VERTICAL_FOV,
	FIXED_HORIZONTAL_FOV
};

enum AspectRatio {
	AR_16_9,
	AR_16_10,
	AR_4_3,
};

class Camera : public Component {
public:
	Camera();
	Camera(GameObject* gameObject);
	~Camera();

	void Update() override;
	void OnEditor() override;

	void SetFixedFOV(FixedFOV fixedFOV);
	void AdjustFieldOfView();
	void AdjustFieldOfView(float width, float height);
	void SetVerticalFieldOfView(float verticalFOV, float screen_width, float screen_height);
	void SetHorizontalFieldOfView(float horizontalFOV, float screen_width, float screen_height);
	void SetPosition(float3 position);
	void SetReference(float3 reference);

	void Look(float3 spot);
	Frustum GetFrustum();

	float* GetViewMatrix();
	float* GetProjectionMatrix();
	bool ContainsAABB(AABB& aabb);
	//virtual void Enable() override;
	//virtual void Disable() override;

public:
	FixedFOV fixedFOV;

private:
	Frustum _frustum;
	AspectRatio _aspectRatio;
	float3 _reference;
};
