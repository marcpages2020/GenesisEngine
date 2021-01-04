#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "FileSystem.h"
#include "Application.h"
#include "GnJSON.h"

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
		GLchar name[64];
		glGetActiveUniform(program, (GLuint)i, 64, &length, &uniform.size, &uniform.type, name);

		uniform.name = name;

		if (uniform.type == GL_FLOAT || uniform.type == GL_INT) 
		{
			uniform.uniformType = UniformType::NUMBER;
			uniform.number = 0.0f;
		}

		else if (uniform.type == GL_BOOL) {
			uniform.uniformType = UniformType::BOOLEAN;
			uniform.boolean = false;
		}

		else if (uniform.type == GL_FLOAT_VEC2 || uniform.type == GL_INT_VEC2 || uniform.type == GL_DOUBLE_VEC2) {
			uniform.uniformType = UniformType::VEC_2;
			uniform.vec2 = float2().zero;
		}

		else if (uniform.type == GL_FLOAT_VEC3 || uniform.type == GL_INT_VEC3 || uniform.type == GL_DOUBLE_VEC3) {
			uniform.uniformType = UniformType::VEC_3;
			uniform.vec3 = float3().zero;
		}

		else if (uniform.type == GL_FLOAT_VEC4 || uniform.type == GL_INT_VEC4 || uniform.type == GL_DOUBLE_VEC4) {
			uniform.uniformType = UniformType::VEC_4;
			uniform.vec4 = float4().zero;
		}

		else if (uniform.type == GL_SAMPLER_2D)
			uniform.uniformType = UniformType::TEXTURE;

		else 
			uniform.uniformType = UniformType::UNKNOWN;

		std::map<std::string, Uniform>::iterator it;
		it = shader->uniforms.find(uniform.name);

		if (it != shader->uniforms.end()) {
			uniform = shader->uniforms[uniform.name];
		}
			
		uniforms.push_back(uniform);
		
	}

	shader->uniforms.clear();

	for (size_t i = 0; i < uniforms.size(); i++)
	{
		shader->uniforms[uniforms[i].name] = uniforms[i];
	}
}

uint ShaderImporter::Save(ResourceShader* shader, char** fileBuffer)
{
	char* buffer;

	GnJSONObj base_object;

	shader->Save(base_object);

	uint size = base_object.Save(&buffer);
	*fileBuffer = buffer;
	return size;
}

bool ShaderImporter::Load(char* fileBuffer, ResourceShader* shader)
{
	char* buffer;
	FileSystem::Load(shader->assetsFile.c_str(), &buffer);

	Import(buffer, shader, shader->assetsFile.c_str());

	GnJSONObj uniforms(fileBuffer);
	shader->Load(uniforms);

	RELEASE_ARRAY(buffer);

	return true;
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

	char* buffer;
	uint size = 0;
	size = Save(shader, &buffer);

	FileSystem::Save(shader->libraryFile.c_str(), buffer, size);

	RELEASE_ARRAY(vertexShaderBuffer);
	RELEASE_ARRAY(fragmentShaderBuffer);
	RELEASE_ARRAY(buffer);
}

void ShaderImporter::CreateShaderAsset(const char* asset_path)
{
	std::string vertex_shader_path = asset_path;
	vertex_shader_path.append(".vert");

	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"layout (location = 2) in vec3 texCoord;\n"
		"layout (location = 3) in vec3 normal;\n"
		"layout (location = 4) in vec3 tangent;\n"
		"uniform mat4 model_matrix;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f);\n"
		"}\0";
	
	FileSystem::Save(vertex_shader_path.c_str(), vertexShaderSource, strlen(vertexShaderSource));

	std::string fragment_shader_path = asset_path;
	fragment_shader_path.append(".frag");

	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);\n"
		"}\0";

	FileSystem::Save(fragment_shader_path.c_str(), fragmentShaderSource, strlen(fragmentShaderSource));
}
