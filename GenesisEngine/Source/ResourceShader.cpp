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
	if (ImGui::CollapsingHeader("Uniforms")) 
	{
		const float step = 1.0f;
		ImGui::Spacing();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			switch (uniforms[i].uniformType)
			{
			case UniformType::BOOLEAN:
					break;
			case UniformType::NUMBER:
				ImGui::InputScalar(uniforms[i].name, ImGuiDataType_Float, &uniforms[i].number, &step);
				break;
			case UniformType::VEC_2:
				ImGui::InputFloat2(uniforms[i].name, uniforms[i].vec2.ptr());
				break;
			case UniformType::VEC_3:
				ImGui::InputFloat3(uniforms[i].name, uniforms[i].vec3.ptr());
				break;
			case UniformType::VEC_4:
				ImGui::InputFloat4(uniforms[i].name, uniforms[i].vec4.ptr());
				break;
			default:
				break;
			}
		}

		if (ImGui::Button("Save Uniforms"))
			App->resources->SaveResource(this);
	}
}

uint ResourceShader::Save(GnJSONObj& base_object)
{
	GnJSONArray gn_uniforms = base_object.AddArray("uniforms");

	for (size_t i = 0; i < uniforms.size(); i++)
	{
		GnJSONObj uniform;

		uniform.AddInt("type", (int)uniforms[i].uniformType);
		uniform.AddString("name", uniforms[i].name);

		switch (uniforms[i].uniformType)
		{
		case UniformType::BOOLEAN:
			uniform.AddBool("value", uniforms[i].boolean);
			break;
		case UniformType::NUMBER:
			uniform.AddFloat("value", uniforms[i].number);
			break;
		case UniformType::VEC_2:
			uniform.AddFloat2("value", uniforms[i].vec2);
			break;
		case UniformType::VEC_3:
			uniform.AddFloat3("value", uniforms[i].vec3);
			break;
		case UniformType::VEC_4:
			uniform.AddFloat4("value", uniforms[i].vec4);
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
		strcpy(uniform.name,  uniform_object.GetString("name", "no_name"));

		switch (uniform.uniformType)
		{
		case UniformType::BOOLEAN:
			uniform.boolean = uniform_object.GetBool("value");
			break;
		case UniformType::NUMBER:
			uniform.number = uniform_object.GetFloat("value");
			break;
		case UniformType::VEC_2:
			uniform.vec2 = uniform_object.GetFloat2("value");
			break;
		case UniformType::VEC_3:
			uniform.vec3 = uniform_object.GetFloat3("value");
			break;
		case UniformType::VEC_4:
			uniform.vec4 = uniform_object.GetFloat4("value");
			break;
		default:
			break;
		}

		uniforms.push_back(uniform);
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
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		switch (uniforms[i].type)
		{
		case GL_BOOL:
			SetBool(uniforms[i].name, uniforms[i].boolean);
			break;

		//FLOAT ======================================================================================
		case GL_FLOAT:
			SetFloat(uniforms[i].name, uniforms[i].number);
			break;
		case GL_FLOAT_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].vec2.x, uniforms[i].vec2.y);
			break;
		case GL_FLOAT_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].vec3.x, uniforms[i].vec3.y, uniforms[i].vec3.z);
			break;
		case GL_FLOAT_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].vec4.x, uniforms[i].vec4.y, uniforms[i].vec4.z, uniforms[i].vec4.z);
			break;

		//INT ======================================================================================
		case GL_INT:
			SetInt(uniforms[i].name, uniforms[i].number);
			break;
		case GL_INT_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].vec2.x, uniforms[i].vec2.y);
			break;
		case GL_INT_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].vec3.x, uniforms[i].vec3.y, uniforms[i].vec3.z);
			break;
		case GL_INT_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].vec4.x, uniforms[i].vec4.y, uniforms[i].vec4.z, uniforms[i].vec4.z);
			break;

		//DOUBLE ====================================================================================
		case GL_DOUBLE:
			SetFloat(uniforms[i].name, uniforms[i].number);
			break;
		case GL_DOUBLE_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].vec2.x, uniforms[i].vec2.y);
			break;
		case GL_DOUBLE_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].vec3.x, uniforms[i].vec3.y, uniforms[i].vec3.z);
			break;
		case GL_DOUBLE_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].vec4.x, uniforms[i].vec4.y, uniforms[i].vec4.z, uniforms[i].vec4.z);
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
