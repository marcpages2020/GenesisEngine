#pragma once
#include "Globals.h"
#include "Resource.h"

typedef unsigned char GLubyte;
class GnJSONObj;

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	uint Save(GnJSONObj& base_object) override;
	//void Load(GnJSONObj& base_object) override;

public:
	uint id;
	int width;
	int height;
	GLubyte* data;
};
