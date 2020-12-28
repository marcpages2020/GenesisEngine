#ifndef _RESOURCE_SHADER_H_
#define _RESOURCE_SHADER_H_

#include "Globals.h"
#include "Resource.h"

#include <vector>

typedef unsigned int GLenum;

enum class ShaderType
{
	VERTEX_SHADER,
	FRAGMENT_SHADER,
	UNKNOWN_SHADER
};

enum class UniformType
{
	BOOLEAN,
	NUMBER,
	VEC_2,
	VEC_3,
	VEC_4,
	TEXTURE,
	UNKNOWN
};

struct Uniform
{
	int size = 0;
	GLenum type;
	UniformType uniformType = UniformType::UNKNOWN;

	bool boolean = false;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	char name[16];
};

class ResourceShader : public Resource {
public:
	ResourceShader(uint UID);
	~ResourceShader();

	void OnEditor();

	void Use();

	void SetUniforms();

	void SetBool(const char* name, bool value);
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVec2(const char* name, float x, float y);
	void SetVec3(const char* name, float x, float y, float z);
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat4(const char* name, float* matrix);

public:
	uint vertexShader;
	uint fragmentShader;
	uint id;

	std::vector<Uniform> uniforms;
};

#endif