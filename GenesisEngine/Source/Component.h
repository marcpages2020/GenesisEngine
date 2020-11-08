#pragma once

class GameObject;

class GnJSONObj;
class GnJSONArray;

enum ComponentType {
	TRANSFORM,
	MESH,
	MATERIAL
};

class Component {
public: 
	Component();
	Component(GameObject* gameObject);
	virtual ~Component();
	virtual void Update();
	virtual void Enable();
	virtual void Disable();

	virtual void Save(GnJSONArray& save_array) {};
	virtual void Load(GnJSONObj& load_object) {};

	bool IsEnabled();
	ComponentType GetType();
	virtual void OnEditor() = 0;

	void SetGameObject(GameObject* gameObject);
	GameObject* GetGameObject();

protected:
	ComponentType type;
	GameObject* _gameObject;
	bool enabled;
};
