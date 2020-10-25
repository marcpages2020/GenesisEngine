#pragma once

class GameObject;

enum class ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL
};

class Component {
public: 
	Component();
	~Component();
	virtual void Update();
	virtual void Enable();
	virtual void Disable();

	ComponentType GetType();
	virtual void OnEditor() = 0;

	void SetGameObject(GameObject* gameObject);
	GameObject* GetGameObject();

protected:
	ComponentType type;
	bool enabled;
	GameObject* gameObject;
};
