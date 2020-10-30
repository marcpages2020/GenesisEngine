#pragma once
#include "Component.h"
#include "Globals.h"

#include <string>

typedef unsigned char GLubyte;
class GnMesh;

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
	Material(GnMesh* mesh, GnTexture* diffuse_texture);
	~Material();
	void OnEditor() override;

	void SetTexture(GnTexture* texture);
	bool DeleteTexture();

private:
	GnTexture* diffuse_texture;
	GnMesh* mesh;
	bool checkers_image;
};