#include "ResourceShader.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), shaderProgram(-1) 
{}

ResourceShader::~ResourceShader()
{}
