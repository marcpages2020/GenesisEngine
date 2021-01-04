#pragma once
#include "Component.h"
#include "Globals.h"

#include <string>

class GnMesh;
class ResourceMaterial;
class ResourceTexture;
class ResourceShader;
class GnJSONArray;
enum class TextureType;

class Material : public Component {
public: 
	Material();
	Material(GameObject* gameObject);
	~Material();

	void SetResourceUID(uint UID) override;
	void BindTexture(ResourceTexture* texture);
	void UseShader();

	void Save(GnJSONArray& save_array) override;
	void Load(GnJSONObj& load_object) override;

	void OnEditor() override;
	ResourceTexture* DrawTextureInformation(ResourceTexture* texture, TextureType type);

	void SetTexture(ResourceTexture* texture);
	void AssignCheckersImage();
	ResourceTexture* GetDiffuseTexture();

public: 
	ResourceShader* shader;

private:
	bool checkers_image;
	bool colored;

	ResourceMaterial* _resource;
	uint checkersID;
};