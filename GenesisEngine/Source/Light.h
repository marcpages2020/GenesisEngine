
#pragma once
#include "Color.h"
#include "glmath.h"
#include "Component.h"

class GnJSONArray;
class GnJSONObj;
class GameObject;

class Light : public Component {
public:
	Light();
	Light(GameObject* gameObject);
	~Light();

	void Init();
	void Update() override;
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();
	void OnEditor() override;
	void Save(GnJSONArray& save_array);
	void Load(GnJSONObj& load_object);

public:
	vec3 position;
	Color ambient;
	Color diffuse;

	int ref;
};