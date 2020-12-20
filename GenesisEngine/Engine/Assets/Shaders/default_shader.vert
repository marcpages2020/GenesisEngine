#version 330 core
in vec3 position;
in vec3 color;
in vec2 texCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
void main()
{
 gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
 ourColor = color;
 TexCoord = texCoord;
}
