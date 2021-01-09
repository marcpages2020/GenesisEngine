#pragma once
#include "Component.h"
#include "Globals.h"
#include "ResourceTexture.h"

#include <string>

class GnMesh;
class ResourceMaterial;
class ResourceShader;
class GnJSONArray;

class Material : public Component {
public: 
	Material();
	Material(GameObject* gameObject);
	~Material();

	void SetResourceUID(uint UID) override;
	void SetShader(ResourceShader* new_shader);
	void BindTexture(ResourceTexture* texture);
	void UseShader();

	void Save(GnJSONArray& save_array) override;
	void Load(GnJSONObj& load_object) override;

	void OnEditor() override;
	ResourceTexture* DrawTextureInformation(ResourceTexture* texture, TextureType type);

	void SetTexture(ResourceTexture* texture, TextureType type = TextureType::DIFFUSE_MAP);
	void AssignCheckersImage();
	ResourceTexture* GetDiffuseTexture();

public: 
	ResourceShader* shader;
	uint shaderID;

private:
	bool checkers_image;
	bool colored;

	ResourceMaterial* _resource;
	uint checkersID;
};