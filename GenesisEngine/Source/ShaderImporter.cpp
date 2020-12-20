#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "FileSystem.h"

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
}

/*
void ShaderImporter::CompilePairingShader(ResourceShader* shader)
{
	std::string pairingShaderPath = FindPairingShader(shader->assetsFile.c_str());

	if (FileSystem::Exists(pairingShaderPath.c_str()))
	{
		char* buffer;
		uint size = FileSystem::Load(pairingShaderPath.c_str(), &buffer);

		Compile(buffer, shader);

		RELEASE_ARRAY(buffer);
	}
}
*/

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

	if (shader->vertexShader != -1)
		glAttachShader(shaderProgram, shader->vertexShader);

	if (shader->fragmentShader != -1)
		glAttachShader(shaderProgram, shader->fragmentShader);

	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (success == 0)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		LOG_ERROR("Error linking shader program: %s", infoLog);
	}
	else
	{
		shader->shaderProgram = shaderProgram;
	}
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
