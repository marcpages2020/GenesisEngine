#pragma once
#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject;

class Camera : public Component {
public:
	Camera(GameObject* gameObject);
	~Camera();

	virtual void Update() override;
	//virtual void Enable() override;
	//virtual void Disable() override;

	void OnEditor() override;
private:
	Frustum _frustum;
};
