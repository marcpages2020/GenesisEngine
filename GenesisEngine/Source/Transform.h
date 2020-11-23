#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GnMesh;
class GnJSONArray;

class Transform : public Component {
public:
	Transform();
	Transform(float3 position, Quat rotation, float3 scale);
	~Transform();

	void Update() override;
	void OnEditor() override;

	void Save(GnJSONArray& save_array) override;
	void Load(GnJSONObj& load_object) override;

	void Set(float4x4 transform);

	float4x4 GetLocalTransform();
	float4x4 GetGlobalTransform();
	void SetGlobalTransform(float4x4 newTransform);

	void UpdateLocalTransform();
	void UpdateTRS();
	void UpdateGlobalTransform();
	void UpdateGlobalTransform(float4x4 parentGlobalTransform);
	void ChangeParentTransform(float4x4 newParentGlobalTransform);

	void Reset();

	void SetPosition(float x, float y, float z);
	void SetPosition(float3 new_position);
	float3 GetPosition();

	void SetRotation(float x, float y, float z);
	void SetRotation(Quat new_rotation);
	void SetRotation(float i, float j, float k, float w);
	Quat GetRotation();
	void UpdateEulerRotation();

	void SetScale(float x, float y, float z);
	void SetScale(float3 new_scale);
	void SetProportionalScale(float multiplier);
	float3 GetScale();

private:
	float4x4 _localTransform = float4x4::identity;
	float4x4 _globalTransform = float4x4::identity;
	float4x4 _parentGlobalTransform = float4x4::identity;

	float3 _position;
	Quat _rotation;
	float3 _scale;
	float3 _eulerRotation;
};

#endif //__TRANSFORM_H__