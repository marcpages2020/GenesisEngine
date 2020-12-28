#include "ResourceShader.h"
#include "glew/include/glew.h"

#include "ImGui/imgui.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), id(-1) 
{}

ResourceShader::~ResourceShader()
{}

void ResourceShader::OnEditor()
{
	if (ImGui::CollapsingHeader("Uniforms")) 
	{
		const float step = 1.0f;
		ImGui::Spacing();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			float values[4] = { uniforms[i].x , uniforms[i].y , uniforms[i].z , uniforms[i].w };

			switch (uniforms[i].uniformType)
			{
			case UniformType::BOOLEAN:
					break;
			case UniformType::NUMBER:
				ImGui::InputScalar(uniforms[i].name, ImGuiDataType_Float, &uniforms[i].x, &step);
				break;
			case UniformType::VEC_2:
				if (ImGui::InputFloat2(uniforms[i].name, values)) {
					uniforms[i].x = values[0];
					uniforms[i].y = values[1];
				}
				break;
			case UniformType::VEC_3:
				if (ImGui::InputFloat3(uniforms[i].name, values)) {
					uniforms[i].x = values[0];
					uniforms[i].y = values[1];
					uniforms[i].z = values[2];
				}
				break;
			case UniformType::VEC_4:
				if (ImGui::InputFloat4(uniforms[i].name, values)) {
					uniforms[i].x = values[0];
					uniforms[i].y = values[1];
					uniforms[i].z = values[2];
					uniforms[i].w = values[3];
				}
				break;
			default:
				break;
			}
		}
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
			SetFloat(uniforms[i].name, uniforms[i].x);
			break;
		case GL_FLOAT_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].x, uniforms[i].y);
			break;
		case GL_FLOAT_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z);
			break;
		case GL_FLOAT_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z, uniforms[i].w);
			break;

		//INT ======================================================================================
		case GL_INT:
			SetInt(uniforms[i].name, uniforms[i].x);
			break;
		case GL_INT_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].x, uniforms[i].y);
			break;
		case GL_INT_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z);
			break;
		case GL_INT_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z, uniforms[i].w);
			break;

		//DOUBLE ====================================================================================
		case GL_DOUBLE:
			SetFloat(uniforms[i].name, uniforms[i].x);
			break;
		case GL_DOUBLE_VEC2:
			SetVec2(uniforms[i].name, uniforms[i].x, uniforms[i].y);
			break;
		case GL_DOUBLE_VEC3:
			SetVec3(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z);
			break;
		case GL_DOUBLE_VEC4:
			SetVec4(uniforms[i].name, uniforms[i].x, uniforms[i].y, uniforms[i].z, uniforms[i].w);
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
