#pragma once
#include "Globals.h"
#include "Resource.h"

typedef unsigned char GLubyte;
class GnJSONObj;

class ResourceTexture : public Resource {
public:
	ResourceTexture(uint UID);
	~ResourceTexture();

	uint SaveMeta(GnJSONObj& base_object, uint last_modification) override;
	//void Load(GnJSONObj& base_object) override;

public:
	uint id;
	int width;
	int height;
	GLubyte* data;
};
