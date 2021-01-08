#ifndef _RESOURCE_SHADER_H_
#define _RESOURCE_SHADER_H_

#include "Globals.h"
#include "Resource.h"

#include <map>
#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

typedef unsigned int GLenum;
class GnJSONObj;
class Material;
class ResourceMaterial;

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
	bool color = false;

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
	void OnEditor(Material* material, ResourceMaterial* resourceMaterial);
	uint Save(GnJSONObj& base_object) override;
	void Load(GnJSONObj& base_object) override;

	void Use();

	void SetUniforms();
	void UpdateUniforms(Material* material, ResourceMaterial* resourceMaterial);
	bool IsDefaultUniform(const char* uniform_name);

	void SetBool(const char* name, bool value);
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVec2(const char* name, float x, float y);
	void SetVec3(const char* name, float x, float y, float z);
	void SetVec4(const char* name, float x, float y, float z, float w);
	void SetMat3(const char* name, float* matrix);
	void SetMat4(const char* name, float* matrix);

public:
	uint vertexShader;
	uint fragmentShader;
	uint program_id;

	std::map<std::string, Uniform> uniforms;
};

#endif