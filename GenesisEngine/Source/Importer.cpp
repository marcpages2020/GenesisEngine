#include "Globals.h"

#include "ResourceModel.h"
#include "ResourceMesh.h"

#include "Importer.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

int Importer::ImportModel(char* fileBuffer, uint fileSize, ResourceModel* model)
{
	const aiScene* scene = aiImportFileFromMemory(fileBuffer, fileSize, aiProcessPreset_TargetRealtime_MaxQuality, NULL);

	if (!scene) {
		LOG_ERROR("Error loading in assimp.");
		aiReleaseImport(scene);
		return -1;
	}

	if (!scene->HasMeshes()) {
		LOG_ERROR("File has no meshes, it cannot be imported");
		aiReleaseImport(scene);
		return -1;
	}

	//Import meshes
	for (size_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];

		ResourceMesh* myMesh = new ResourceMesh(i);
		model->AddMesh(ImportMesh(scene->mMeshes[i], myMesh));
		model->meshesResources.push_back(myMesh);
	}

	//Import materials
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* aimaterial = scene->mMaterials[i];
		//TODO: Import materials
	}

	aiReleaseImport(scene);

	return model->GetUID();
}

int Importer::ProcessAssimpNode(const aiScene* scene, aiNode* ainode, aiNode* parentAiNode, ModelNode* parentNode, ResourceModel* model)
{
	int ret = 0;

	ModelNode modelNode;;

	//Assign node name
	modelNode.name = ainode->mName.C_Str();

	//Assign node mesh and material
	if (ainode->mMeshes != nullptr)
	{
		//Mesh --------------------------------------------------------------
		modelNode.meshID = model->GetMeshAt(*ainode->mMeshes);

		//Materials ----------------------------------------------------------
		//aiMesh* aimesh = scene->mMeshes[*ainode->mMeshes];
		//modelNode.materialID = model->materials[aimesh->mMaterialIndex];
	}

	for (size_t i = 0; i < ainode->mNumChildren; i++)
	{
		ret = ProcessAssimpNode(scene, ainode->mChildren[i], ainode, &modelNode, model);
	}

	return ret;
}

int Importer::ImportMesh(aiMesh* aimesh, ResourceMesh* mesh)
{
	//Name
	mesh->name = new char[aimesh->mName.length];
	strcpy(mesh->name, aimesh->mName.C_Str());

	mesh->numVertices = aimesh->mNumVertices;
	mesh->vertices = new float[mesh->numVertices * VERTEX_ATTRIBUTES];

	//vertices, normals, color and texture coordinates
	for (size_t v = 0; v < aimesh->mNumVertices; v++)
	{
		//vertices copying
		mesh->vertices[v * VERTEX_ATTRIBUTES] = aimesh->mVertices[v].x;
		mesh->vertices[v * VERTEX_ATTRIBUTES + 1] = aimesh->mVertices[v].y;
		mesh->vertices[v * VERTEX_ATTRIBUTES + 2] = aimesh->mVertices[v].z;

		//color copying
		if (aimesh->HasVertexColors(v))
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + 3] = aimesh->mColors[v]->r;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 4] = aimesh->mColors[v]->g;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 5] = aimesh->mColors[v]->b;
		}
		else
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + 3] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 4] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 5] = 0.0f;
		}

		if (aimesh->mTextureCoords[0])
		{
			//texcoords copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + 6] = aimesh->mTextureCoords[0][v].x;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 7] = aimesh->mTextureCoords[0][v].y;

			//tangets copying
			if (aimesh->mTangents != nullptr)
			{
				mesh->vertices[v * VERTEX_ATTRIBUTES + 11] = aimesh->mTangents[v].x;
				mesh->vertices[v * VERTEX_ATTRIBUTES + 12] = aimesh->mTangents[v].y;
				mesh->vertices[v * VERTEX_ATTRIBUTES + 13] = aimesh->mTangents[v].z;
			}
		}
		else
		{
			//texcoords copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + 6] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 7] = 0.0f;

			//tangets copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + 11] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 12] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 13] = 0.0f;
		}

		//normal copying
		if (aimesh->HasNormals())
		{
			//normal copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + 8] = aimesh->mNormals[v].x;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 9] = aimesh->mNormals[v].y;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 10] = aimesh->mNormals[v].z;
		}
		else
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + 8] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 9] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + 10] = 0.0f;
		}
	}

	//indices copying
	if (aimesh->HasFaces())
	{
		mesh->numIndices = aimesh->mNumFaces * 3;
		mesh->indices = new uint[mesh->numIndices]();
		LOG("%s imported with %d indices", aimesh->mName.C_Str(), mesh->numIndices);

		for (size_t f = 0; f < aimesh->mNumFaces; f++)
		{
			if (aimesh->mFaces[f].mNumIndices != 3) {
				LOG_WARNING("Geometry face with != 3 indices!"); }
			else {
				memcpy(&mesh->indices[f * 3], aimesh->mFaces[f].mIndices, 3 * sizeof(uint)); }
		}
	}

	mesh->GenerateBuffers();

	LOG("New mesh %s with %d vertices", mesh->name, mesh->numVertices);

	return mesh->GetUID();
}
