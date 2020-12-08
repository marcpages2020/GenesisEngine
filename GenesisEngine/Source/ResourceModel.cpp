#include "ResourceModel.h"
#include "Application.h"
#include "GnJSON.h"
#include "Importers.h"
#include "Camera.h"
#include "Light.h"

ResourceModel::ResourceModel(uint UID) : Resource(UID, ResourceType::RESOURCE_MODEL) {}

ResourceModel::~ResourceModel()
{
	nodes.clear();
	meshes.clear();
	materials.clear();
	textures.clear();

	for (size_t i = 0; i < lights.size(); i++)
	{
		lights[i] = nullptr;
	}
	lights.clear();

	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i] = nullptr;
	}
	cameras.clear();
}

uint ResourceModel::Save(GnJSONObj& base_object)
{
	return 1;
}

uint ResourceModel::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddString("Library path", App->resources->GetLibraryPath(_uid));
	base_object.AddInt("lastModified", last_modification);

	ModelImportingOptions importingOptions = App->resources->modelImportingOptions;

	base_object.AddFloat("global scale", (float)importingOptions.globalScale);
	base_object.AddInt("forward axis", (int)importingOptions.forwardAxis);
	base_object.AddInt("up axis", (int)importingOptions.upAxis);
	base_object.AddBool("ignore cameras", importingOptions.ignoreCameras);
	base_object.AddBool("ignore lights", importingOptions.ignoreLights);

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
			node_object.AddInt("MeshID", nodes[i].meshID);
			node_object.AddString("mesh_library_path", App->resources->GetLibraryPath(nodes[i].meshID));
		}

		if (nodes[i].materialID != -1)
		{
			node_object.AddInt("MaterialID", nodes[i].materialID);
			node_object.AddString("material_library_path", App->resources->GetLibraryPath(nodes[i].materialID));
		}

		nodes_array.AddObject(node_object);
	}

	GnJSONArray lights_array = base_object.AddArray("Lights");
	for (size_t i = 0; i < lights.size(); i++)
	{
		lights[i]->Save(lights_array);
	}

	GnJSONArray cameras_array = base_object.AddArray("Cameras");
	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i]->Save(cameras_array);
	}

	return 1;
}
