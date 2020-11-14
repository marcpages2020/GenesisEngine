#include "ResourceModel.h"
#include "GnJSON.h"

ResourceModel::ResourceModel(uint UID) : Resource(UID, ResourceType::RESOURCE_MODEL) {}

ResourceModel::~ResourceModel(){}

uint ResourceModel::Save(GnJSONObj& base_object)
{
	base_object.AddInt("UID", _uid);
	return 1;
}
