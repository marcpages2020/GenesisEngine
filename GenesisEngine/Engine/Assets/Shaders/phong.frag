#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform float time;

void main()
{
    color = texture(ourTexture, TexCoord);
    //color = vec4(vec3(sin(time * 2.0), 0.0, 0.0), 1.0);
}