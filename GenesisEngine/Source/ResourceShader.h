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

	void SetBool(const char* name, bool value);
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetMat4(const char* name, float* matrix);

public:
	uint vertexShader;
	uint fragmentShader;
	uint id;
};

#endif