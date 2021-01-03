#include "MeshImporter.h"

#include "Timer.h"
#include "ResourceMesh.h"

#include "Assimp/Assimp/include/cimport.h"
#include "Assimp/Assimp/include/scene.h"
#include "Assimp/Assimp/include/postprocess.h"

void MeshImporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void MeshImporter::CleanUp()
{
	aiDetachAllLogStreams();
}

void MeshImporter::Import(const aiMesh* aimesh, ResourceMesh* mesh)
{
	Timer timer;
	timer.Start();

	//indices copying
	if (aimesh->HasFaces())
	{
		mesh->indices_amount = aimesh->mNumFaces * 3;
		mesh->indices = new uint[mesh->indices_amount]();
		LOG("%s imported with %d indices", aimesh->mName.C_Str(), mesh->indices_amount);

		for (size_t f = 0; f < aimesh->mNumFaces; f++)
		{
			if (aimesh->mFaces[f].mNumIndices != 3)
			{
				LOG_WARNING("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&mesh->indices[f * 3], aimesh->mFaces[f].mIndices, 3 * sizeof(uint));
			}
		}
	}

	//vertex copying
	mesh->vertices_amount = aimesh->mNumVertices;
	mesh->vertices = new float[mesh->vertices_amount * VERTEX_ATTRIBUTES];

	//LOG("%s imported with %d vertices", aimesh->mName.C_Str(), mesh->vertices_amount);

	//normals, color and texture coordinates
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

	//LOG("Mesh imported in %d ms", timer.Read());
}

uint64 MeshImporter::Save(ResourceMesh* mesh, char** fileBuffer)
{
	
	uint ranges[2] = { mesh->indices_amount, mesh->vertices_amount }; // mesh->normals_amount

	uint size = sizeof(ranges) + sizeof(uint) * mesh->indices_amount + sizeof(float) * mesh->vertices_amount * VERTEX_ATTRIBUTES;

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//store indices
	bytes = sizeof(uint) * mesh->indices_amount;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//store vertices
	bytes = sizeof(float) * mesh->vertices_amount * VERTEX_ATTRIBUTES;
	memcpy(cursor, mesh->vertices, bytes);
	//cursor += bytes;

	*fileBuffer = buffer;

	return size;
}

bool MeshImporter::Load(char* fileBuffer, ResourceMesh* mesh, uint size)
{

	bool ret = true;

	Timer timer;
	timer.Start();

	char* cursor = fileBuffer;

	uint ranges[2];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	mesh->indices_amount = ranges[0];
	mesh->vertices_amount = ranges[1];

	// Load indices
	bytes = sizeof(uint) * mesh->indices_amount;
	mesh->indices = new uint[mesh->indices_amount];
	memcpy(mesh->indices, cursor, bytes);
	cursor += bytes;

	//load vertices
	bytes = sizeof(float) * mesh->vertices_amount * VERTEX_ATTRIBUTES;
	mesh->vertices = new float[mesh->vertices_amount * VERTEX_ATTRIBUTES];
	memcpy(mesh->vertices, cursor, bytes);
	//cursor += bytes;

	//LOG("%s loaded in %d ms", mesh->libraryFile.c_str(), timer.Read());

	mesh->GenerateBuffers();

	return ret;
}