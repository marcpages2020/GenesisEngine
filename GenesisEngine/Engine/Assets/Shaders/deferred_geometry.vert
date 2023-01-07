#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

void main()
{
	TexCoord = texCoord;
	Normal = mat3(transpose(inverse(model_matrix))) * normals;
	FragPos = vec3(model_matrix * vec4(position, 1.0));
	
	gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
}




















