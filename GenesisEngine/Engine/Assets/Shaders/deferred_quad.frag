#version 330 core

in vec2 vTexCoord;

uniform sampler2D uColor;
uniform sampler2D uNormals;
uniform sampler2D uPosition;
uniform sampler2D uDepth;

out vec4 oColor;

void main()
{
    vec3 vColor      = vec3(texture(uColor, vTexCoord));
    vec3 vNormal     = normalize(vec3(texture(uNormals, vTexCoord)));
    vec3 vPosition   = vec3(texture(uPosition, vTexCoord));

    oColor = vec4(vColor, 1.0f);
    //oColor = vec4(vec3(1.0, 0.0, 0.0), 1.0f);
}




















