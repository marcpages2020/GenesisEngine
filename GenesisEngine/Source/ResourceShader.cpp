#include "ResourceShader.h"
#include "glew/include/glew.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), id(-1) 
{}

ResourceShader::~ResourceShader()
{}

void ResourceShader::Use()
{
	glUseProgram(id);
}
