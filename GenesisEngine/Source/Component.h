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
	virtual ~Component();
	virtual void Update();
	virtual void Enable();
	virtual void Disable();

	bool IsEnabled();
	ComponentType GetType();
	virtual void OnEditor() = 0;

	void SetGameObject(GameObject* gameObject);
	GameObject* GetGameObject();

protected:
	ComponentType type;
	GameObject* gameObject;
	bool enabled;
};
