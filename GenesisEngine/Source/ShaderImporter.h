#ifndef _SHADER_IMPORTER_H_
#define _SHADER_IMPORTER_H_

class ResourceShader;
typedef unsigned int GLuint;
enum class ShaderType;
struct Uniform;

#include "Globals.h"
#include <string>
#include <vector>

namespace ShaderImporter 
{
	void Import(char* fileBuffer, ResourceShader* shader, const char* assets_path);
	GLuint Compile(char* fileBuffer, ShaderType type);
	bool ShaderHasError(GLuint shader, ShaderType type);
	void CreateProgram(ResourceShader* shader);
	void SetUniforms(GLuint program, ResourceShader* shader);
	uint Save(ResourceShader* shader, char** fileBuffer);
	bool Save(ResourceShader* shader, const char* vertexShader, const  char* fragmentShader);
	bool Load(char* fileBuffer, ResourceShader* shader);
	std::string FindPairingShader(const char* current_shader_path);
	ShaderType GetTypeFromPath(const char* path);
	bool RecompileShader(const char* vertexShaderBuffer, const char* fragmentShaderBuffer, ResourceShader* shader);
	void CreateShaderAsset(const char* asset_path); //introduce the path with the asset name included
}

#endif
