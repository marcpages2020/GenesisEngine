#include "ResourceModel.h"
#include "GnJSON.h"

ResourceModel::ResourceModel(uint UID) : Resource(UID, ResourceType::RESOURCE_MODEL) {}

ResourceModel::~ResourceModel(){}

uint ResourceModel::Save(GnJSONObj& base_object)
{
	return 1;
}

uint ResourceModel::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddInt("lastModified", last_modification);
	return 1;
}
