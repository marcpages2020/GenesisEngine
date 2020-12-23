#ifndef _RESOURCE_SHADER_H_
#define _RESOURCE_SHADER_H_

#include "Globals.h"
#include "Resource.h"

enum class ShaderType
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	UNKNOWN_SHADER
};

class ResourceShader : public Resource {
public:
	ResourceShader(uint UID);
	~ResourceShader();

	void Use();

public:
	int vertexShader;
	int fragmentShader;
	int id;
};

#endif