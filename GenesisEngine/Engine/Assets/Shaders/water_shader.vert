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

uniform float speed;
float waveHeight;
uniform vec2 direction;

void main()
{

 vec3 fPosition = position;
 
 float A = 0.05;
 float Q = 0.5f;
 float speed = 0.1f;
 float wave_length = 0.1f;
 float pi = 3.1415f;
 float phase_constant = speed * 2.0 * pi / wave_length;
 vec2 Direction = vec2(1.0, 0.5);
 float w = 10.5f;
 
 vec3 wave1;
 float amp1 = 0.1f;
 vec3 dir1 = normalize(vec3(0.25, 0.5, 0.6));
 
 wave1.x = Q * A * dir1.x * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.y = Q * A * dir1.y * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.z = A * sin(dot(w * dir1, position) + phase_constant * time);
 
 vec3 wave2;
 float amp2 = 0.1f;
 vec3 dir2 = normalize(vec3(0.65, 0.25, 0.6));
 
 wave2.x = Q * A * dir1.x * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.y = Q * A * dir1.y * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.z = A * sin(dot(w * dir2, position) + phase_constant * time);
 
 fPosition += wave1 + wave2;
 
 gl_Position = projection * view * model_matrix * vec4(fPosition, 1.0);
  
 vec3 diffuse = vec3(0.2, 0.725, 0.9);
 diffuse.gb += fPosition.z * 2.0;


  ourColor = diffuse;
}




























