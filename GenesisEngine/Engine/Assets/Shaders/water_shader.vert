#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

vec3 fNormal;

uniform float time;
uniform float waveHeight;

void main()
{
  vec4 pos = projection * view * model_matrix * vec4(position, 1.0f);
  vec3 fPosition = pos.xyz;

  fNormal = normals;
  fNormal.z += sin(time * 2.0 * sqrt(position.x * position.x + position.y * position.y));
  fPosition.z += fNormal.z * 0.02;

  vec3 worldPosition = position;
  worldPosition += fNormal * 0.025 + waveHeight;

  gl_Position = projection * view * model_matrix * vec4(worldPosition, 1.0);
  
  vec3 lightPos = vec3(0.5, 1.0, 0.5);
  vec3 ambient = vec3(0.1, 0.15, 0.1);
  vec3 diffuse = vec3(0.2, 0.6, 0.8);
  vec3 specular = vec3(0.6, 0.6, 0.6);
  float shiny = 15.0;
  vec3 specularity = vec3(0.0);  

  float intensity = dot(fNormal, lightPos);

  if(intensity > 0.0)
  {
   vec3 eye = vec3(1.0, 1.0, 1.0);
   vec3 h = normalize(lightPos + eye);
   float intensity_specular = dot(h, fNormal);
   specularity = specular * pow(intensity_specular, shiny);
  }

  ourColor = max(intensity * diffuse + specularity, ambient);
}
















