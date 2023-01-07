#include "ShaderImporter.h"
#include "ResourceShader.h"
#include "FileSystem.h"
#include "Engine.h"
#include "GnJSON.h"
#include "WindowShaderEditor.h"

#include "glad/include/glad/glad.h"

void ShaderImporter::Import(char* fileBuffer, ResourceShader* shader, const char* assets_path)
{
	ShaderType type = GetTypeFromPath(assets_path);
	char* buffer;

	std::string pairingShader = FindPairingShader(assets_path);

	if (FileSystem::Load(pairingShader.c_str(), &buffer) == 0)
		return;

	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;

	if (type == ShaderType::VERTEX_SHADER)
	{
	    vertexShader = Compile(fileBuffer, type);
		fragmentShader = Compile(buffer, ShaderType::FRAGMENT_SHADER);

		Save(shader, fileBuffer, buffer);
	}
	else if (type == ShaderType::FRAGMENT_SHADER)
	{
		fragmentShader = Compile(fileBuffer, type);
		vertexShader = Compile(buffer, ShaderType::VERTEX_SHADER);

		Save(shader, buffer, fileBuffer);
	}

	if (vertexShader != 0 && fragmentShader != 0)
	{
		shader->vertexShader = vertexShader;
		shader->fragmentShader = fragmentShader;
		CreateProgram(shader);
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

		if (!ShaderHasError(vertexShader, ShaderType::VERTEX_SHADER))
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

		if (!ShaderHasError(fragmentShader, ShaderType::FRAGMENT_SHADER))
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

bool ShaderImporter::ShaderHasError(GLuint shader, ShaderType type)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == 0)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);

		if (type == ShaderType::VERTEX_SHADER) {
			LOG_ERROR("Error compilating vertex shader: %s", infoLog);}
		else {
			LOG_ERROR("Error compilating fragment shader: %s", infoLog);}

		WindowShaderEditor* shader_editor = dynamic_cast<WindowShaderEditor*>(engine->editor->GetWindow("Shader Editor"));
		shader_editor->SetErrorsOnScreen(infoLog, type);
	}
	else
	{
		if (type == ShaderType::VERTEX_SHADER) {
			LOG("Vertex shader compiled properly");}
		else {
			LOG("Fragment shader compiled properly");}
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
	}
	else
	{
		shader->program_id = shaderProgram;
		
		SetUniforms(shaderProgram, shader);

		LOG("Shader program created porperly");
	}

	glDetachShader(shaderProgram, shader->vertexShader);
	glDetachShader(shaderProgram, shader->fragmentShader);

	glDeleteShader(shader->vertexShader);
	glDeleteShader(shader->fragmentShader);

	shader->vertexShader = -1;
	shader->fragmentShader = -1;
}

void ShaderImporter::SetUniforms(GLuint program, ResourceShader* shader)
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

bool ShaderImporter::Save(ResourceShader* shader, const char* vertexShader, const char* fragmentShader)
{
	char* vertexShaderBuffer;
	char* fragmentShaderBuffer;

	char vertexShaderPath[128];
	char fragmentShaderPath[128];

	sprintf_s(vertexShaderPath, 128, "Library/Shaders/%d.vert", shader->GetUID());
	sprintf_s(fragmentShaderPath, 128, "Library/Shaders/%d.frag", shader->GetUID());

	FileSystem::Save(vertexShaderPath, vertexShader, strlen(vertexShader));
	FileSystem::Save(fragmentShaderPath, fragmentShader, strlen(fragmentShader));

	return true;
}


bool ShaderImporter::Load(char* fileBuffer, ResourceShader* shader)
{
	char* vertexShaderBuffer;
	char* fragmentShaderBuffer;

	char vertexShaderPath[128];
	char fragmentShaderPath[128];

	sprintf_s(vertexShaderPath, 128, "Library/Shaders/%d.vert", shader->GetUID());
	sprintf_s(fragmentShaderPath, 128, "Library/Shaders/%d.frag", shader->GetUID());

	FileSystem::Load(vertexShaderPath, &vertexShaderBuffer);
	FileSystem::Load(fragmentShaderPath, &fragmentShaderBuffer);

	GLuint vertexShader = Compile(vertexShaderBuffer, ShaderType::VERTEX_SHADER);
	GLuint fragmentShader = Compile(fragmentShaderBuffer, ShaderType::FRAGMENT_SHADER);

	if (vertexShader != 0 && fragmentShader != 0)
	{
		shader->vertexShader = vertexShader;
		shader->fragmentShader = fragmentShader;
		CreateProgram(shader);
	}

	GnJSONObj uniforms(fileBuffer);
	shader->Load(uniforms);

	RELEASE_ARRAY(vertexShaderBuffer);
	RELEASE_ARRAY(fragmentShaderBuffer);

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

	return std::string("NoPairingShader");
}

ShaderType ShaderImporter::GetTypeFromPath(const char* path)
{
	std::string path_str = path;

	if (path_str.find(".vert") != std::string::npos)
		return ShaderType::VERTEX_SHADER;
	else if (path_str.find(".frag") != std::string::npos)
		return ShaderType::FRAGMENT_SHADER;
}

bool ShaderImporter::RecompileShader(const char* vertexShaderBuffer, const char* fragmentShaderBuffer, ResourceShader* shader)
{
	bool ret = true;

	if (shader == nullptr) {
		LOG_ERROR("Shader: %s could not be recompiled. It was not found");
		return false;
	}

	GLuint vertexShader = Compile((char*)vertexShaderBuffer, ShaderType::VERTEX_SHADER);
	GLuint fragmentShader = Compile((char*)fragmentShaderBuffer, ShaderType::FRAGMENT_SHADER);

	if (vertexShader != 0 && fragmentShader != 0)
	{
		shader->vertexShader = vertexShader;
		shader->fragmentShader = fragmentShader;

		glDeleteProgram(shader->program_id);
		CreateProgram(shader);

		char* buffer;
		uint size = 0;
		size = Save(shader, &buffer);

		FileSystem::Save(shader->libraryFile.c_str(), buffer, size);
		RELEASE_ARRAY(buffer);
	}
	else
		ret = false;

	return ret;
}

void ShaderImporter::CreateShaderAsset(const char* asset_path)
{
	std::string vertex_shader_path = asset_path;
	vertex_shader_path.append(".vert");

	const char* vertexShaderSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 color;\n"
		"layout (location = 2) in vec2 texCoord;\n"
		"layout (location = 3) in vec3 normal;\n"
		"layout (location = 4) in vec3 tangent;\n"
		"\n"
		"uniform mat4 model_matrix;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"uniform mat3 normalMatrix;\n"
		"\n"
		"out vec3 Normal;\n"
		"\n"
		"void main()\n"
		"{\n"
		"Normal = normalMatrix * normal;\n"
		"gl_Position = projection * view * model_matrix * vec4(position, 1.0f);\n"
		"}\0";
	
	FileSystem::Save(vertex_shader_path.c_str(), vertexShaderSource, strlen(vertexShaderSource));

	std::string fragment_shader_path = asset_path;
	fragment_shader_path.append(".frag");

	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec2 TexCoord;\n"
		"in vec3 Normal;\n"
		"in vec3 FragPos;\n"
		"\n"
		"out vec4 color;\n"
		"\n"
		"uniform vec3 cameraPosition;\n"
		"uniform bool hasDiffuseMap;\n"
		"\n"
		"struct Material {\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"	float shininess;\n"
		"};\n"
		"\n"
		"struct Light {\n"
		"	vec3 position;\n"
		"	vec3 ambient;\n"
		"	vec3 diffuse;\n"
		"	vec3 specular;\n"
		"};\n"
		"\n"
		"Light light;\n"
		"\n"
		"Material material;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	material.ambient = vec3(0.35, 0.35, 0.35);\n"
		"	material.diffuse = vec3(0.75, 0.75, 0.75);\n"
		"	material.specular = vec3(0.65, 0.65, 0.65);\n"
		"\n"
		"	light.ambient = vec3(0.2, 0.2, 0.2);\n"
		"	light.position = vec3(0.0, 10.0, 0.0);\n"
		"	light.diffuse = vec3(0.5, 0.5, 0.5);\n"
		"	light.specular = vec3(0.2, 0.2, 0.2);\n"
		"\n"
		"	// ambient\n"
		"	vec3 ambient = light.ambient * material.ambient;\n"
		"\n"
		"	// diffuse \n"
		"	vec3 norm = normalize(Normal);\n"
		"	vec3 lightDir = normalize(light.position - FragPos);\n"
		"	float diff = max(dot(norm, lightDir), 0.0);\n"
		"	vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
		"\n"
		"	// specular\n"
		"	vec3 viewDir = normalize(cameraPosition - FragPos);\n"
		"	vec3 reflectDir = reflect(-lightDir, norm);\n"
		"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
		"	vec3 specular = light.specular * (spec * material.specular);\n"
		"\n"
		"	color = vec4(material.ambient + diffuse + specular, 1.0);\n"
		"}\n";

	FileSystem::Save(fragment_shader_path.c_str(), fragmentShaderSource, strlen(fragmentShaderSource));
}
