#include "ResourceModel.h"
#include "Application.h"
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
	base_object.AddString("Library path", App->resources->GetLibraryPath(_uid));
	base_object.AddInt("lastModified", last_modification);

	GnJSONArray nodes_array = base_object.AddArray("Nodes");

	for (size_t i = 0; i < nodes.size(); i++)
	{
		GnJSONObj node_object;

		node_object.AddString("Name", nodes[i].name.c_str());
		node_object.AddInt("UID", nodes[i].UID);
		node_object.AddInt("Parent UID", nodes[i].parentUID);
		
		node_object.AddFloat3("Position", nodes[i].position);
		node_object.AddQuaternion("Rotation", nodes[i].rotation);
		node_object.AddFloat3("Scale", nodes[i].scale);

		if (nodes[i].meshID != -1)
		{
			node_object.AddInt("Mesh UID", nodes[i].meshID);
			node_object.AddString("Mesh library path", App->resources->GetLibraryPath(nodes[i].meshID));
		}

		if (nodes[i].materialID != -1)
		{
			node_object.AddInt("Material UID", nodes[i].materialID);
			node_object.AddString("Material library path", App->resources->GetLibraryPath(nodes[i].materialID));
		}

		nodes_array.AddObject(node_object);
	}

	return 1;
}
