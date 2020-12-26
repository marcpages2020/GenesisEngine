#include "ResourceShader.h"
#include "glew/include/glew.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), id(-1) 
{}

ResourceShader::~ResourceShader()
{}

void ResourceShader::Use()
{
	int success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	glUseProgram(id);
}

void ResourceShader::SetBool(const char* name, bool value)
{
	glUniform1i(glGetUniformLocation(id, name), (int)value);
}

void ResourceShader::SetInt(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(id, name), value);
}

void ResourceShader::SetFloat(const char* name, float value)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniform1f(variableLoc, value);
}

void ResourceShader::SetMat4(const char* name, float* matrix)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniformMatrix4fv(variableLoc, 1, GL_FALSE, matrix);
}
