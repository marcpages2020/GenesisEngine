#include "ResourceShader.h"
#include "glad/include/glad/glad.h"
#include "Engine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Time.h"
#include "Material.h"
#include "ResourceMaterial.h"

#include "GnJSON.h"
#include "ImGui/imgui.h"

ResourceShader::ResourceShader(uint UID) : Resource(UID, ResourceType::RESOURCE_SHADER), vertexShader(-1), fragmentShader(-1), program_id(-1) 
{}

ResourceShader::~ResourceShader()
{
	glDeleteProgram(program_id);

	vertexShader = -1;
	fragmentShader = -1;
	program_id = -1;
}

uint ResourceShader::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddInt("lastModified", last_modification);
	base_object.AddString("Library path", libraryFile.c_str());

	return _uid;
}

void ResourceShader::OnEditor(Material* material, ResourceMaterial* resourceMaterial)
{
	
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_DefaultOpen;
	if (ImGui::TreeNodeEx("Uniforms", tree_flags))
	{
		const float step = 1.0f;
		ImGui::Spacing();

		for (std::map<std::string, Uniform>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
		{
			if (!IsDefaultUniform(it->second.name.c_str()))
			{
				switch (it->second.uniformType)
				{
				case UniformType::BOOLEAN:
					ImGui::Checkbox(it->first.c_str(), &it->second.boolean);
					break;

				case UniformType::NUMBER:
					ImGui::InputScalar(it->first.c_str(), ImGuiDataType_Float, &it->second.number, &step);
					break;

				case UniformType::VEC_2:
					ImGui::InputFloat2(it->first.c_str(), it->second.vec2.ptr());
					break;

				case UniformType::VEC_3:
					
					ImGui::PushID(it->first.c_str());
					ImGui::Checkbox("Color", &it->second.color);
					ImGui::SameLine();
					if (it->second.color)
					{
						ImVec4 color = ImVec4(it->second.vec3.x, it->second.vec3.y, it->second.vec3.z, 1.0);
						if (ImGui::ColorEdit3("Color##1", (float*)&color)) {
							it->second.vec3 = float3(color.x, color.y, color.z);
						}
					}
					else {
						ImGui::InputFloat3(it->first.c_str(), it->second.vec3.ptr());
					}
					ImGui::PopID();
					
					break;

				case UniformType::VEC_4:
					
					ImGui::PushID(it->first.c_str());
					ImGui::Checkbox("Color", &it->second.color);
					ImGui::SameLine();

					if (it->second.color)
					{
						ImVec4 color = ImVec4(it->second.vec4.x, it->second.vec4.y, it->second.vec4.z, it->second.vec4.w);
						if (ImGui::ColorEdit4("Color##1", (float*)&color)) {
							it->second.vec4 = float4(color.x, color.y, color.z, color.w);
						}
					}
					else {
						ImGui::InputFloat4(it->first.c_str(), it->second.vec4.ptr());
					}
					ImGui::PopID();
					
					break;
				default:
					break;
			}
			}
		}

		ImGui::Spacing();
		if (ImGui::Button("Save Uniforms"))
			engine->resources->SaveResource(this);

		ImGui::Spacing();
		ImGui::TreePop();
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
			uniform.AddBool("color", it->second.color);
			break;
		case UniformType::VEC_4:
			uniform.AddFloat4("value", it->second.vec4);
			uniform.AddBool("color", it->second.color);
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
			uniform.number = uniform_object.GetFloat("value", 0.0f);
			break;
		case UniformType::VEC_2:
			uniform.type = GL_FLOAT_VEC2;
			uniform.vec2 = uniform_object.GetFloat2("value", float2().zero);
			break;
		case UniformType::VEC_3:
			uniform.type = GL_FLOAT_VEC3;
			uniform.vec3 = uniform_object.GetFloat3("value", float3().zero);
			uniform.color = uniform_object.GetBool("color", false);
			break;
		case UniformType::VEC_4:
			uniform.type = GL_FLOAT_VEC4;
			uniform.vec4 = uniform_object.GetFloat4("value", float4().zero);
			uniform.color = uniform_object.GetBool("color", false);
			break;
		default:
			break;
		}

		uniforms[uniform.name] = uniform;
	}
}

void ResourceShader::Use()
{
	//int success = 0;
	//glGetProgramiv(program_id, GL_LINK_STATUS, &success);

	glUseProgram(program_id);

	//SetUniforms();
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
			if (it->first == "diffuseColor") 
				it->second.color = true;
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

void ResourceShader::UpdateUniforms(Material* material, ResourceMaterial* resourceMaterial)
{
	float4x4 globalTransform = material->GetGameObject()->GetTransform()->GetGlobalTransform();

	for (std::map<std::string, Uniform>::iterator it = uniforms.begin(); it != uniforms.end(); it++)
	{
		//Default uniforms
		if (IsDefaultUniform(it->first.c_str()))
		{
			if (it->first == "model_matrix") {
				SetMat4("model_matrix", globalTransform.Transposed().ptr());
			}
			else if (it->first == "projection") 
			{
				SetMat4("projection", engine->camera->GetProjectionMatrixM().Transposed().ptr());
			}
			else if (it->first == "view") 
			{
				SetMat4("view", engine->camera->GetViewMatrixM().Transposed().ptr());
			}
			else if (it->first == "time") 
			{
				SetFloat("time", Time::realClock.timeSinceStartup());
			}
			else if (it->first == "normalMatrix") 
			{
				float3x3 normalMatrix = globalTransform.Float3x3Part();
				normalMatrix.Inverse();
				normalMatrix.Transpose();
				SetMat3("normalMatrix", normalMatrix.ptr());
			}
			else if (it->first == "cameraPosition") 
			{
				float3 cameraPosition = engine->camera->GetPosition();
				SetVec3("cameraPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
			} 
			else if (it->first == "diffuseColor") 
			{
				if (resourceMaterial != nullptr) {
					SetVec4("diffuseColor", resourceMaterial->diffuseColor.r, resourceMaterial->diffuseColor.g, resourceMaterial->diffuseColor.b, resourceMaterial->diffuseColor.a);
				}
			}
		}
		//User Uniforms
		else {
			switch (it->second.uniformType)
			{
			case UniformType::BOOLEAN:
				SetBool(it->first.c_str(), it->second.boolean);
				break;
			case UniformType::NUMBER:
				SetFloat(it->first.c_str(), it->second.number);
				break;
			case UniformType::VEC_2:
				SetVec2(it->first.c_str(), it->second.vec2.x, it->second.vec2.y);
				break;
			case UniformType::VEC_3:
				SetVec3(it->first.c_str(), it->second.vec3.x, it->second.vec3.y, it->second.vec3.z);
				break;
			case UniformType::VEC_4:
				SetVec4(it->first.c_str(), it->second.vec4.x, it->second.vec4.y, it->second.vec4.z, it->second.vec4.w);
				break;
			default:
				break;
			}
		}
	}
}

bool ResourceShader::IsDefaultUniform(const char* uniform_name)
{
	const char* default_uniforms[11] = { "model_matrix" , "projection", "view", "time", "normalMatrix","cameraPosition", "diffuseMapTiling",
										 "normalMapTiling", "hasDiffuseMap", "hasNormalMap", "diffuseColor"};

	for (size_t i = 0; i < 11; i++)
	{
		if (strcmp(uniform_name, default_uniforms[i]) == 0)
			return true;
	}

	return false;
}

void ResourceShader::AddUniform(Uniform uniform)
{
	uniforms[uniform.name] = uniform;
}

void ResourceShader::SetBool(const char* name, bool value)
{
	glUniform1i(glGetUniformLocation(program_id, name), (int)value);
}

void ResourceShader::SetInt(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(program_id, name), value);
}

void ResourceShader::SetFloat(const char* name, float value)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniform1f(variableLoc, value);
}

void ResourceShader::SetVec2(const char* name, float x, float y)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniform2f(variableLoc, (GLfloat)x, (GLfloat)y);
}

void ResourceShader::SetVec3(const char* name, float x, float y, float z)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniform3f(variableLoc, (GLfloat)x, (GLfloat)y, (GLfloat)z);
}

void ResourceShader::SetVec4(const char* name, float x, float y, float z, float w)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniform4f(variableLoc, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}

void ResourceShader::SetMat3(const char* name, float* matrix)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniformMatrix3fv(variableLoc, 1, GL_FALSE, matrix);
}

void ResourceShader::SetMat4(const char* name, float* matrix)
{
	GLint variableLoc = glGetUniformLocation(program_id, name);

	glUniformMatrix4fv(variableLoc, 1, GL_FALSE, matrix);
}

uint ResourceShader::GetHandle()
{
	return program_id;
}

Uniform::Uniform()
{
	boolean = false;
}

Uniform::Uniform(const char* g_name, UniformType g_uniformType, bool g_color)
{
	name = g_name;
	uniformType = g_uniformType;
	color = g_color;
}
