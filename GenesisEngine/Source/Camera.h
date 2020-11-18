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

	void AdjustAspecRatio();
	void SetPosition(float3 position);
	void SetReference(float3 reference);

	float* GetViewMatrix();
	//virtual void Enable() override;
	//virtual void Disable() override;

private:
	Frustum _frustum;
	AspectRatio _aspectRatio;
	float3 _reference;
};
