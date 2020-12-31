#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;
uniform float time;

void main()
{
    color = vec4(ourColor, 1.0);
}


































