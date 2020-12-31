#include "ResourceShader.h"
#include "glew/include/glew.h"
#include "Application.h"

#include "GnJSON.h"
#include "ImGui/imgui.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), id(-1) 
{}

ResourceShader::~ResourceShader()
{}

uint ResourceShader::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddInt("lastModified", last_modification);
	base_object.AddString("Library path", libraryFile.c_str());

	return _uid;
}

void ResourceShader::OnEditor()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::CollapsingHeader("Uniforms", flags)) 
	{
		const float step = 1.0f;
		ImGui::Spacing();
		for (std::map<std::string, Uniform>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
		{
			switch (it->second.uniformType)
			{
			case UniformType::BOOLEAN:
					break;
			case UniformType::NUMBER:
				ImGui::InputScalar(it->first.c_str(), ImGuiDataType_Float, &it->second.number, &step);
				break;
			case UniformType::VEC_2:
				ImGui::InputFloat2(it->first.c_str(), it->second.vec2.ptr());
				break;
			case UniformType::VEC_3:
				ImGui::InputFloat3(it->first.c_str(), it->second.vec3.ptr());
				break;
			case UniformType::VEC_4:
				ImGui::InputFloat4(it->first.c_str(), it->second.vec4.ptr());
				break;
			default:
				break;
			}
		}

		if (ImGui::Button("Save Uniforms"))
			App->resources->SaveResource(this);

		ImGui::Spacing();
	}
}

uint ResourceShader::Save(GnJSONObj& base_object)
{
	GnJSONArray gn_uniforms = base_object.AddArray("uniforms");

	for (std::map<std::string, Uniform>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
	{
		GnJSONObj uniform;

		uniform.AddInt("type", (int)it->second.uniformType);
		uniform.AddString("name", it->first.c_str());

		switch (it->second.uniformType)
		{
		case UniformType::BOOLEAN:
			uniform.AddBool("value", it->second.boolean);
			break;
		case UniformType::NUMBER:
			uniform.AddFloat("value", it->second.number);
			break;
		case UniformType::VEC_2:
			uniform.AddFloat2("value", it->second.vec2);
			break;
		case UniformType::VEC_3:
			uniform.AddFloat3("value", it->second.vec3);
			break;
		case UniformType::VEC_4:
			uniform.AddFloat4("value", it->second.vec4);
			break;
		case UniformType::TEXTURE:
			break;
		default:
			break;
		}

		gn_uniforms.AddObject(uniform);
	}

	return 1;
}

void ResourceShader::Load(GnJSONObj& base_object)
{
	GnJSONArray gn_uniforms = base_object.GetArray("uniforms");

	for (size_t i = 0; i < gn_uniforms.Size(); i++)
	{
		Uniform uniform;
		GnJSONObj uniform_object = gn_uniforms.GetObjectAt(i);

		uniform.uniformType = (UniformType)uniform_object.GetInt("type");
		uniform.name = uniform_object.GetString("name", "no_name");

		switch (uniform.uniformType)
		{
		case UniformType::BOOLEAN:
			uniform.type = GL_BOOL;
			uniform.boolean = uniform_object.GetBool("value");
			break;
		case UniformType::NUMBER:
			uniform.type = GL_FLOAT;
			uniform.number = uniform_object.GetFloat("value");
			break;
		case UniformType::VEC_2:
			uniform.type = GL_FLOAT_VEC2;
			uniform.vec2 = uniform_object.GetFloat2("value");
			break;
		case UniformType::VEC_3:
			uniform.type = GL_FLOAT_VEC3;
			uniform.vec3 = uniform_object.GetFloat3("value");
			break;
		case UniformType::VEC_4:
			uniform.type = GL_FLOAT_VEC4;
			uniform.vec4 = uniform_object.GetFloat4("value");
			break;
		default:
			break;
		}

		uniforms[uniform.name] = uniform;
	}
}

void ResourceShader::Use()
{
	int success = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	glUseProgram(id);

	SetUniforms();
}

void ResourceShader::SetUniforms()
{
	for (std::map<std::string, Uniform>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
	{
		switch (it->second.type)
		{
		case GL_BOOL:
			SetBool(it->first.c_str(), it->second.boolean);
			break;

		//FLOAT ======================================================================================
		case GL_FLOAT:
			SetFloat(it->first.c_str(), it->second.number);
			break;
		case GL_FLOAT_VEC2:
			SetVec2(it->first.c_str(), it->second.vec2.x, it->second.vec2.y);
			break;
		case GL_FLOAT_VEC3:
			SetVec3(it->first.c_str(), it->second.vec3.x, it->second.vec3.y, it->second.vec3.z);
			break;
		case GL_FLOAT_VEC4:
			SetVec4(it->first.c_str(), it->second.vec4.x, it->second.vec4.y, it->second.vec4.z, it->second.vec4.z);
			break;

		//INT ======================================================================================
		case GL_INT:
			SetInt(it->first.c_str(), it->second.number);
			break;
		case GL_INT_VEC2:
			SetVec2(it->first.c_str(), it->second.vec2.x, it->second.vec2.y);
			break;
		case GL_INT_VEC3:
			SetVec3(it->first.c_str(), it->second.vec3.x, it->second.vec3.y, it->second.vec3.z);
			break;
		case GL_INT_VEC4:
			SetVec4(it->first.c_str(), it->second.vec4.x, it->second.vec4.y, it->second.vec4.z, it->second.vec4.z);
			break;

		//DOUBLE ====================================================================================
		case GL_DOUBLE:
			SetFloat(it->first.c_str(), it->second.number);
			break;
		case GL_DOUBLE_VEC2:
			SetVec2(it->first.c_str(), it->second.vec2.x, it->second.vec2.y);
			break;
		case GL_DOUBLE_VEC3:
			SetVec3(it->first.c_str(), it->second.vec3.x, it->second.vec3.y, it->second.vec3.z);
			break;
		case GL_DOUBLE_VEC4:
			SetVec4(it->first.c_str(), it->second.vec4.x, it->second.vec4.y, it->second.vec4.z, it->second.vec4.z);
			break;
		default:
			break;
		}
	}
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

void ResourceShader::SetVec2(const char* name, float x, float y)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniform2f(variableLoc, (GLfloat)x, (GLfloat)y);
}

void ResourceShader::SetVec3(const char* name, float x, float y, float z)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniform3f(variableLoc, (GLfloat)x, (GLfloat)y, (GLfloat)z);
}

void ResourceShader::SetVec4(const char* name, float x, float y, float z, float w)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniform4f(variableLoc, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}

void ResourceShader::SetMat4(const char* name, float* matrix)
{
	GLint variableLoc = glGetUniformLocation(id, name);

	glUniformMatrix4fv(variableLoc, 1, GL_FALSE, matrix);
}

Uniform::Uniform()
{
	boolean = false;
}
