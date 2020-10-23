#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"

class Transform : public Component {
public:
	Transform();
	Transform(float3 position, Quat rotation, float3 scale);
	~Transform();

	void Update() override;

private:
	float3 position;
	float3 scale;
	Quat rotation;
};