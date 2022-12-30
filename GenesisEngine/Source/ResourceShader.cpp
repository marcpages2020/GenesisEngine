#include "ResourceShader.h"

ResourceShader::ResourceShader(uint newUid) : Resource(newUid, ResourceType::SHADER), handle(-1)
{}

uint ResourceShader::GetHandle() const
{
	return handle;
}

void ResourceShader::SetHandle(uint newHandle)
{
	handle = newHandle;
}
