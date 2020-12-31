#ifndef _RESOURCE_SHADER_H_
#define _RESOURCE_SHADER_H_

#include "Globals.h"
#include "Resource.h"

#include <map>
#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

typedef unsigned int GLenum;
class GnJSONObj;

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
	Uniform();
	int size = 0;
	GLenum type;
	UniformType uniformType = UniformType::UNKNOWN;
	
	union
	{
		bool boolean;
		float number;
		float2 vec2;
		float3 vec3;
		float4 vec4;
	};

	std::string name;
};

class ResourceShader : public Resource {
public:
	ResourceShader(uint UID);
	~ResourceShader();

	uint SaveMeta(GnJSONObj& base_object, uint last_modification) override;
	void OnEditor();
	uint Save(GnJSONObj& base_object) override;
	void Load(GnJSONObj& base_object) override;

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

	std::map<std::string, Uniform> uniforms;
};

#endif