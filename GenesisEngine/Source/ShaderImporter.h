#ifndef _SHADER_IMPORTER_H_
#define _SHADER_IMPORTER_H_

class ResourceShader;
typedef unsigned int GLuint;
enum class ShaderType;

#include <string>

namespace ShaderImporter 
{
	void Import(char* fileBuffer, ResourceShader* shader, const char* assets_path);
	GLuint Compile(char* fileBuffer, ShaderType type);
	//void CompilePairingShader(ResourceShader* shader);
	bool ShaderHasError(GLuint shader);
	void CreateProgram(ResourceShader* shader);
	std::string FindPairingShader(const char* current_shader_path);
	ShaderType GetTypeFromPath(const char* path);
}

#endif
