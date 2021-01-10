#include "MaterialImporter.h"
#include "Application.h"
#include "Timer.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "GnJSON.h"
#include "FileSystem.h"
#include "Assimp/Assimp/include/material.h"

void MaterialImporter::Import(const aiMaterial* aimaterial, ResourceMaterial* material)
{
	Timer timer;
	timer.Start();

	aiString path;
	aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
	aiColor3D aiDiffuseColor;
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);

	material->diffuseColor.r = aiDiffuseColor.r;
	material->diffuseColor.g = aiDiffuseColor.g;
	material->diffuseColor.b = aiDiffuseColor.b;
	material->diffuseColor.a = 1.0f;

	if (path.length > 0)
	{
		std::string file_path = material->assetsFile;
		path = FileSystem::GetFile(path.C_Str());
		file_path = FindTexture(path.C_Str(), material->assetsFile.c_str());

		if (file_path.size() > 0)
		{
			std::string meta_file = App->resources->GenerateMetaFile(file_path.c_str());
			if (!FileSystem::Exists(meta_file.c_str()))
			{
				material->diffuseMapID = App->resources->ImportFile(file_path.c_str());}
			else 
			{
				material->diffuseMapID = App->resources->GetUIDFromMeta(meta_file.c_str());}

			if (material->diffuseMapID == 0)
				LOG("Texture %s not found", file_path.c_str());
		}

		//LOG("%s imported in %.3f s", texture->path.c_str(), timer.ReadSec());
	}
}

uint64 MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	GnJSONObj base_object;
	base_object.AddColor("diffuseColor", material->diffuseColor);

	base_object.AddInt("diffuseMapID", material->diffuseMapID);
	base_object.AddFloat2("diffuseMapTiling", float2(material->tiling[DIFFUSE_MAP][0], material->tiling[DIFFUSE_MAP][1]));

	base_object.AddInt("normalMapID", material->normalMapID);
	base_object.AddFloat2("normalMapTiling", float2(material->tiling[NORMAL_MAP][0], material->tiling[NORMAL_MAP][1]));


	char* buffer;
	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;

	return size;
}

bool MaterialImporter::Load(const char* fileBuffer, ResourceMaterial* material, uint size)
{
	bool ret = true;
	Timer timer;
	timer.Start();

	GnJSONObj material_data(fileBuffer);

	material->diffuseColor = material_data.GetColor("diffuseColor", Color(0.0f, 1.0f, 0.0f, 1.0f));

	//diffuse map
	uint diffuseMapID = material_data.GetInt("diffuseMapID", 0);

	if (diffuseMapID != 0)
	{
		material->diffuseMap = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(diffuseMapID));
		material->diffuseMapID = diffuseMapID;
		float2 diffuseMapTiling = material_data.GetFloat2("diffuseMapTiling");
		material->tiling[DIFFUSE_MAP][0] = diffuseMapTiling.x;
		material->tiling[DIFFUSE_MAP][1] = diffuseMapTiling.y;
	}

	//normal map
	uint normalMapID = material_data.GetInt("normalMapID", 0);

	if (normalMapID != 0)
	{
		material->normalMap = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(normalMapID));
		material->normalMapID = normalMapID;
		float2 normalMapTiling = material_data.GetFloat2("normalMapTiling");
		material->tiling[NORMAL_MAP][0] = normalMapTiling.x;
		material->tiling[NORMAL_MAP][1] = normalMapTiling.y;
	}

	material_data.Release();
	return ret;
}

std::string MaterialImporter::FindTexture(const char* texture_name, const char* model_directory)
{
	std::string path;
	FileSystem::SplitFilePath(model_directory, &path);
	std::string texture_path = path + texture_name;

	//Check if the texture is in the same folder
	if (FileSystem::Exists(texture_path.c_str()))
	{
		return texture_path.c_str();
	}
	else
	{
		//Check if the texture is in a sub folder
		texture_path = path + "Textures/" + texture_name;
		if (FileSystem::Exists(texture_path.c_str()))
		{
			return texture_path.c_str();
		}
		else
		{
			//Check if the texture is in the root textures folder
			texture_path = std::string("Assets/Textures/") + texture_name;
			if (FileSystem::Exists(texture_path.c_str()))
			{
				return texture_path.c_str();
			}
		}
	}

	texture_path.clear();
	return texture_path;
}

bool MaterialImporter::DeleteTexture(const char* material_library_path)
{
	bool ret = true;

	char* buffer = nullptr;
	FileSystem::Load(material_library_path, &buffer);

	GnJSONObj material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("diffuseTexture");
	const char* texture_library_path = App->resources->Find(diffuseTextureUID);

	if (texture_library_path != nullptr)
	{
		FileSystem::Delete(texture_library_path);
		App->resources->ReleaseResource(diffuseTextureUID);
		App->resources->ReleaseResourceData(diffuseTextureUID);
	}
	else
		LOG_WARNING("Texture: %s could not be deleted. Not found", material_library_path);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return ret;
}

const char* MaterialImporter::ExtractTexture(const char* material_library_path)
{
	char* buffer = nullptr;
	FileSystem::Load(material_library_path, &buffer);

	GnJSONObj material_data(buffer);

	int diffuseTextureUID = material_data.GetInt("diffuseTexture");
	const char* texture_library_path = App->resources->Find(diffuseTextureUID);

	material_data.Release();
	RELEASE_ARRAY(buffer);

	return texture_library_path;
}
