#pragma once
#include "Resource.h"
class ResourceShader : public Resource
{
public:
	ResourceShader(uint newUid);

	uint GetHandle() const;
	void SetHandle(uint newHandle);

private: 
	uint handle;
};

