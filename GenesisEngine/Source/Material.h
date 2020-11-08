#pragma once
#include "Component.h"
#include "Globals.h"

#include <string>

typedef unsigned char GLubyte;
class GnMesh;
class GnJSONArray;

struct GnTexture 
{
	std::string name = "Unnamed Texture";
	std::string path = "Unknown path";
	uint id = -1;
	GLubyte* data = nullptr;
	int width = -1;
	int height = -1;

	~GnTexture()
	{
		name.clear();
		path.clear();
		//delete data;
		data = nullptr;
	}
};

class Material : public Component {
public: 
	Material();
	Material(GameObject* gameObject);
	Material(GnMesh* mesh, GnTexture* diffuse_texture);
	~Material();

	void Save(GnJSONArray& save_array) override;
	void Load(GnJSONObj& load_object) override;
	void OnEditor() override;

	void SetTexture(GnTexture* texture);
	GnTexture* GetDiffuseTexture();
	void SetMesh(GnMesh* mesh);
	bool DeleteTexture();

private:
	GnTexture* diffuse_texture;
	GnMesh* mesh;
	bool checkers_image;
};