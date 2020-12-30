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
	bool ShaderHasError(GLuint shader);
	void CreateProgram(ResourceShader* shader);
	void GetUniforms(GLuint program, ResourceShader* shader);
	uint Save(ResourceShader* shader, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceShader* shader);
	std::string FindPairingShader(const char* current_shader_path);
	ShaderType GetTypeFromPath(const char* path);
	void RecompileShader(const char* vertexShaderPath, const char* fragmentShaderPath);
}

#endif
