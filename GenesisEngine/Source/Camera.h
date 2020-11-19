#pragma once
#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

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

	void AdjustFieldOfView();
	void AdjustFieldOfView(float width, float height);
	void SetFieldOfView(float verticalFOV, float screen_width, float screen_height);
	void SetPosition(float3 position);
	void SetReference(float3 reference);

	void Look(float3 spot);
	Frustum GetFrustum();

	float* GetViewMatrix();
	bool ContainsAABB(AABB& aabb);
	//virtual void Enable() override;
	//virtual void Disable() override;

private:
	Frustum _frustum;
	AspectRatio _aspectRatio;
	float3 _reference;
};
