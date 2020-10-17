#include "FileSystem.h"

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
			currentMesh->vertices_amount = currentAiMesh->mNumVertices;
			currentMesh->vertices = new float[currentMesh->vertices_amount * 3]();
			memcpy(currentMesh->vertices, currentAiMesh->mVertices, sizeof(float) * currentMesh->vertices_amount * 3);
			LOG("New mesh with %d vertices", currentMesh->vertices_amount);

			if (currentAiMesh->HasFaces()) 
			{
				currentMesh->indices_amount = currentAiMesh->mNumFaces * 3;
				currentMesh->indices = new uint[currentMesh->indices_amount]();

				for (size_t j = 0; j < currentAiMesh->mNumFaces; j++)
				{
					if (currentAiMesh->mFaces[j].mNumIndices != 3) 
					{
						LOG_WARNING("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&currentMesh->indices[j * 3], currentAiMesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				currentMesh->GenerateBuffers();
				collection->meshes.push_back(currentMesh);
			}
		}

		aiReleaseImport(scene);
	}
	else
		LOG_ERROR("Error loading scene %s", path);

	return collection;
}
