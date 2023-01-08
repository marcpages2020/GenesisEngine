#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D diffuseMap;

layout(location = 0) out vec4 albedoTexture; //Albedo 
layout(location = 1) out vec4 normalsTexture; //Normals 
layout(location = 2) out vec4 positionTexture; //Position 

void main()
{
   albedoTexture = texture(diffuseMap, TexCoord);
   normalsTexture = vec4(Normal, 1.0);
   positionTexture = vec4(FragPos, 1.0);
}




















