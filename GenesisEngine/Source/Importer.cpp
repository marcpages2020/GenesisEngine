#include "Globals.h"

#include "ResourceModel.h"
#include "ResourceMesh.h"
#include "ResourceShader.h"

#include "Importer.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include <glad/glad.h>

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
		//VERTICES
		mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_POSITION_OFFSET] = aimesh->mVertices[v].x;
		mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_POSITION_OFFSET + 1] = aimesh->mVertices[v].y;
		mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_POSITION_OFFSET + 2] = aimesh->mVertices[v].z;

		//NORMALS
		if (aimesh->HasNormals())
		{
			//normal copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET] = aimesh->mNormals[v].x;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET + 1] = aimesh->mNormals[v].y;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET + 2] = aimesh->mNormals[v].z;
		}
		else
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET + 1] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_NORMALS_OFFSET + 2] = 0.0f;
		}

		//TEXCOORDS
		if (aimesh->mTextureCoords[0])
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TEXCOORDS_OFFSET] = aimesh->mTextureCoords[0][v].x;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TEXCOORDS_OFFSET + 1] = aimesh->mTextureCoords[0][v].y;
		}
		else
		{
			//texcoords copying
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TEXCOORDS_OFFSET] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TEXCOORDS_OFFSET + 1] = 0.0f;
		}

		//COLOR
		if (aimesh->HasVertexColors(v))
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET] = aimesh->mColors[v]->r;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET + 1] = aimesh->mColors[v]->g;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET + 2] = aimesh->mColors[v]->b;
		}
		else
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET + 1] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_COLOR_OFFSET + 2] = 0.0f;
		}

		//TANGENTS
		if (aimesh->mTangents != nullptr)
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET] = aimesh->mTangents[v].x;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET + 1] = aimesh->mTangents[v].y;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET + 2] = aimesh->mTangents[v].z;
		}
		else
		{
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET + 1] = 0.0f;
			mesh->vertices[v * VERTEX_ATTRIBUTES + VERTEX_TANGENTS_OFFSET + 2] = 0.0f;
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
				LOG_WARNING("Geometry face with != 3 indices!");
			}
			else {
				memcpy(&mesh->indices[f * 3], aimesh->mFaces[f].mIndices, 3 * sizeof(uint));
			}
		}
	}

	mesh->GenerateBuffers();

	LOG("New mesh %s with %d vertices", mesh->name, mesh->numVertices);

	return mesh->GetUID();
}

int Importer::ImportShader(char* fileBuffer, ResourceShader* shader)
{
	GLchar  infoLogBuffer[1024] = {};
	GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
	GLsizei infoLogSize;
	GLint   success;

	char versionString[] = "#version 430\n";
	char vertexShaderDefine[] = "#define VERTEX\n";
	char fragmentShaderDefine[] = "#define FRAGMENT\n";

	const GLchar* vertexShaderSource[] = {
	versionString,
	vertexShaderDefine,
	};

	const GLint vertexShaderLengths[] = {
	(GLint)strlen(versionString),
	(GLint)strlen(vertexShaderDefine),
	};

	const GLchar* fragmentShaderSource[] = {
	versionString,
	fragmentShaderDefine,
	};

	const GLint fragmentShaderLengths[] = {
		(GLint)strlen(versionString),
		(GLint)strlen(fragmentShaderDefine),
	};

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		LOG_ERROR("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", "shader name", infoLogBuffer);
	}

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		LOG_ERROR("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", "shader name", infoLogBuffer);
	}

	GLuint programHandle = glCreateProgram();
	glAttachShader(programHandle, vshader);
	glAttachShader(programHandle, fshader);
	glLinkProgram(programHandle);
	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
		LOG_ERROR("glLinkProgram() failed with program %s\nReported message:\n%s\n", "shader name", infoLogBuffer);
	}

	glUseProgram(0);

	glDetachShader(programHandle, vshader);
	glDetachShader(programHandle, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	shader->SetHandle(programHandle);

	return shader->GetUID();
}
