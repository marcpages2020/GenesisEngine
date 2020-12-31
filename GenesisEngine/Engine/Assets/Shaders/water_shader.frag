#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 color;

uniform sampler2D ourTexture;
uniform float time;

uniform float contrast;
in float relative_position;


void main()
{
    vec3 ambient_color = vec3(0.1, 0.275, 0.4);
    vec4 texture =  texture(ourTexture, TexCoord);
    
    float shiny = relative_position * contrast;
    color = texture;
    
    color.rgb += ambient_color + shiny;
} 











































