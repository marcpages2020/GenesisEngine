#include "FileSystem.h"
#include "PhysFS/include/physfs.h"

#include "Mesh.h"

#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"

#pragma comment (lib, "Assimp/Assimp/libx86/assimp.lib")

void FileSystem::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void FileSystem::CleanUp()
{
	aiDetachAllLogStreams();
}

MeshCollection* FileSystem::LoadFBX(const char* path)
{
	MeshCollection* collection = nullptr;
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		collection = new MeshCollection();

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* currentMesh = new Mesh();
			aiMesh* currentAiMesh = scene->mMeshes[i];

			//vertex copying
			currentMesh->vertices_amount = currentAiMesh->mNumVertices;
			currentMesh->vertices = new float[currentMesh->vertices_amount * 3]();
			memcpy(currentMesh->vertices, currentAiMesh->mVertices, sizeof(float) * currentMesh->vertices_amount * 3);
			LOG("%s loaded with %d vertices", currentAiMesh->mName.C_Str(), currentMesh->vertices_amount);

			//face copying
			if (currentAiMesh->HasFaces()) 
			{
				currentMesh->indices_amount = currentAiMesh->mNumFaces * 3;
				currentMesh->indices = new uint[currentMesh->indices_amount]();

				for (size_t f = 0; f < currentAiMesh->mNumFaces; f++)
				{
					if (currentAiMesh->mFaces[f].mNumIndices != 3) 
					{
						LOG_WARNING("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&currentMesh->indices[f * 3], currentAiMesh->mFaces[f].mIndices, 3 * sizeof(uint));
					}
				}
			}

			//normal copying
			if (currentAiMesh->HasNormals())
			{
				currentMesh->normals = new float[currentAiMesh->mNumVertices * 3]();
				currentMesh->colors = new float[currentMesh->indices_amount * 4]();
				currentMesh->texturecoords = new float[currentAiMesh->mNumVertices * 2]();

				for (size_t v = 0, n = 0, tx = 0, c = 0; v < currentAiMesh->mNumVertices; v++, n += 3, c += 4, tx += 2)
				{
					//normal copying
					currentMesh->normals[n] = currentAiMesh->mNormals[v].x;
					currentMesh->normals[n + 1] = currentAiMesh->mNormals[v].y;
					currentMesh->normals[n + 2] = currentAiMesh->mNormals[v].z;

					//color copying
					if (currentAiMesh->HasVertexColors(v)) 
					{
						currentMesh->colors[v] = currentAiMesh->mColors[v]->r;
						currentMesh->colors[v + 1] = currentAiMesh->mColors[v]->g;
						currentMesh->colors[v + 2] = currentAiMesh->mColors[v]->b;
						currentMesh->colors[v + 3] = currentAiMesh->mColors[v]->a;
					}
					else 
					{
						currentMesh->colors[v] = 0.0f;
						currentMesh->colors[v + 1] = 0.0f;
						currentMesh->colors[v + 2] = 0.0f;
						currentMesh->colors[v + 3] = 0.0f;
					}

					//texturecoords copying
					if (currentAiMesh->HasTextureCoords(v)) 
					{
						currentMesh->texturecoords[tx] = currentAiMesh->mTextureCoords[v]->x;
						currentMesh->texturecoords[tx + 1] = currentAiMesh->mTextureCoords[v]->y;
					}
				}
			}
			
			currentMesh->GenerateBuffers();
			collection->meshes.push_back(currentMesh);
		}

		aiReleaseImport(scene);
	}
	else
		LOG_ERROR("Error loading scene %s", path);

	return collection;
}

