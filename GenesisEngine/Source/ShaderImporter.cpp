#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "FileSystem.h"
#include "Application.h"

#include "glew/include/glew.h"

void ShaderImporter::Import(char* fileBuffer, ResourceShader* shader, const char* assets_path)
{
	ShaderType type = GetTypeFromPath(assets_path);
	char* buffer;

	std::string pairingShader = FindPairingShader(assets_path);
	FileSystem::Load(pairingShader.c_str(), &buffer);

	if (type == ShaderType::VERTEX_SHADER)
	{
		shader->vertexShader = Compile(fileBuffer, type);
		shader->fragmentShader = Compile(buffer, ShaderType::FRAGMENT_SHADER);
	}
	else if (type == ShaderType::FRAGMENT_SHADER)
	{
		shader->fragmentShader = Compile(fileBuffer, type);
		shader->vertexShader = Compile(buffer, ShaderType::VERTEX_SHADER);
	}

	CreateProgram(shader);

	RELEASE_ARRAY(buffer);
}

GLuint ShaderImporter::Compile(char* fileBuffer, ShaderType type)
{
	if (type == ShaderType::VERTEX_SHADER)
	{
		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &fileBuffer, NULL);
		glCompileShader(vertexShader);

		if (!ShaderHasError(vertexShader))
			return vertexShader;
		else
			glDeleteShader(vertexShader);
	}
	else if (type == ShaderType::FRAGMENT_SHADER)
	{
		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fileBuffer, NULL);
		glCompileShader(fragmentShader);

		if (!ShaderHasError(fragmentShader))
			return fragmentShader;
		else
			glDeleteShader(fragmentShader);
	}
	else
	{
		LOG_ERROR("Trying to import unknown shader type");
	}

	return 0;
}

bool ShaderImporter::ShaderHasError(GLuint shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == 0)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		LOG_ERROR("Error compilating shader: %s", infoLog);
	}
	else
	{
		LOG("Shader Compiled properly");
	}

	return success == 0;
}

void ShaderImporter::CreateProgram(ResourceShader* shader)
{
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, shader->vertexShader);
	glAttachShader(shaderProgram, shader->fragmentShader);

	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		LOG_ERROR("Error linking shader program: %s", infoLog);

		glDeleteShader(shader->vertexShader);
		glDeleteShader(shader->fragmentShader);
	}
	else
	{
		shader->id = shaderProgram;
		
		GetUniforms(shaderProgram, shader);

		LOG("Shader program created porperly");

		glDetachShader(shaderProgram, shader->vertexShader);
		glDetachShader(shaderProgram, shader->fragmentShader);

		glDeleteShader(shader->vertexShader);
		glDeleteShader(shader->fragmentShader);
	}
}

void ShaderImporter::GetUniforms(GLuint program, ResourceShader* shader)
{
	int count = 0;
	std::vector<Uniform> uniforms;

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

	for (size_t i = 0; i < count; i++)
	{
		Uniform uniform;
		GLsizei length;
		glGetActiveUniform(program, (GLuint)i, 16, &length, &uniform.size, &uniform.type, (GLchar*)&uniform.name);

		if (strcmp(uniform.name, "model_matrix") != 0 && strcmp(uniform.name, "projection") 
			!= 0 && strcmp(uniform.name, "view") != 0 && strcmp(uniform.name, "time") != 0)
		{
			if (uniform.type == GL_FLOAT || uniform.type == GL_INT)
				uniform.uniformType = UniformType::NUMBER;

			else if (uniform.type == GL_BOOL)
				uniform.uniformType = UniformType::BOOLEAN;

			else if (uniform.type == GL_FLOAT_VEC2 || uniform.type == GL_INT_VEC2 || uniform.type == GL_DOUBLE_VEC2)
				uniform.uniformType = UniformType::VEC_2;

			else if (uniform.type == GL_FLOAT_VEC3 || uniform.type == GL_INT_VEC3 || uniform.type == GL_DOUBLE_VEC3)
				uniform.uniformType = UniformType::VEC_3;

			else if (uniform.type == GL_FLOAT_VEC4 || uniform.type == GL_INT_VEC4 || uniform.type == GL_DOUBLE_VEC4)
				uniform.uniformType = UniformType::VEC_4;

			else if (uniform.type == GL_SAMPLER_2D)
				uniform.uniformType = UniformType::TEXTURE;

			else 
				uniform.uniformType = UniformType::UNKNOWN;

			for (size_t j = 0; j < shader->uniforms.size(); j++)
			{
				if (strcmp(shader->uniforms[j].name, uniform.name) == 0)
					uniform = shader->uniforms[j];
				
			}
			
			uniforms.push_back(uniform);
		}
	}

	shader->uniforms = uniforms;
}

std::string ShaderImporter::FindPairingShader(const char* current_shader_path)
{
	std::string shader_path = current_shader_path;

	if (shader_path.find(".vert") != std::string::npos)
	{
		std::string pairingShaderPath = shader_path;
		pairingShaderPath = pairingShaderPath.substr(0, shader_path.find_last_of("."));
		pairingShaderPath.append(".frag");
		return pairingShaderPath;
	}
	else if (shader_path.find(".frag") != std::string::npos)
	{
		std::string pairingShaderPath = shader_path;
		pairingShaderPath = pairingShaderPath.substr(0, shader_path.find_last_of("."));
		pairingShaderPath.append(".vert");
		return pairingShaderPath;
	}
}

ShaderType ShaderImporter::GetTypeFromPath(const char* path)
{
	std::string path_str = path;

	if (path_str.find(".vert") != std::string::npos)
		return ShaderType::VERTEX_SHADER;
	else if (path_str.find(".frag") != std::string::npos)
		return ShaderType::FRAGMENT_SHADER;
}

void ShaderImporter::RecompileShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ResourceShader* shader = dynamic_cast<ResourceShader*>(App->resources->RequestResource(App->resources->Find(vertexShaderPath)));

	if (shader == nullptr) {
		LOG_ERROR("Shader: %s could not be recompiled. It was not found");
		return;
	}

	char* vertexShaderBuffer;
	FileSystem::Load(vertexShaderPath, &vertexShaderBuffer);
	GLuint vertexShader = Compile(vertexShaderBuffer, ShaderType::VERTEX_SHADER);

	char* fragmentShaderBuffer;
	FileSystem::Load(fragmentShaderPath, &fragmentShaderBuffer);
	GLuint fragmentShader = Compile(fragmentShaderBuffer, ShaderType::FRAGMENT_SHADER);

	if (vertexShader != 0 && fragmentShader != 0)
	{
		shader->vertexShader = vertexShader;
		shader->fragmentShader = fragmentShader;
	}

	CreateProgram(shader);

	RELEASE_ARRAY(vertexShaderBuffer);
	RELEASE_ARRAY(fragmentShaderBuffer);
}
