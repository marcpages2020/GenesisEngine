#include "ResourceTexture.h"
#include "GnJSON.h"

ResourceTexture::ResourceTexture(uint UID) : Resource(UID, ResourceType::RESOURCE_TEXTURE), id(0), width(-1), height(-1), data(nullptr) {}

ResourceTexture::~ResourceTexture() {}

uint ResourceTexture::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddInt("lastModified", last_modification);
	//base_object.AddInt("width", width);
	//base_object.AddInt("height", height);

	return 1;
}

