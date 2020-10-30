#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

#include "MathGeoLib/include/MathGeoLib.h"

class Transform : public Component {
public:
	Transform();
	Transform(float3 position, Quat rotation, float3 scale);
	~Transform();

	void Update() override;
	void OnEditor() override;

	void Set(float4x4 transform);

	float4x4 GetLocalTransform();
	float4x4 GetGlobalTransform();

	void UpdateLocalTransform();
	void UpdateGlobalTransform(float4x4 parentGlobalTransform);

	void Reset();

	void SetPosition(float x, float y, float z);
	void SetPosition(float3 new_position);
	float3 GetPosition();

	void SetRotation(float x, float y, float z);
	void SetRotation(Quat new_rotation);
	void SetRotation(float i, float j, float k, float w);
	Quat GetRotation();

	void SetScale(float x, float y, float z);
	void SetScale(float3 new_scale);
	void SetProportionalScale(float multiplier);
	float3 GetScale();

private:
	float4x4 localTransform;
	float4x4 globalTransform;

	float3 position;
	float3 scale;
	float3 eulerRotation;
	Quat rotation;
};

#endif //__TRANSFORM_H__