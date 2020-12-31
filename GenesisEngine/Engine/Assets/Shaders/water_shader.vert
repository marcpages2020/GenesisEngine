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
uniform vec2 direction;
uniform float wave_length;

void main()
{

 vec3 fPosition = position;
 
 float A = 0.05;
 float Q = 0.5f;
 float speed = 0.15f;
 float pi = 3.1415f;
 float phase_constant = speed * 2.0 * pi / wave_length;
 float w = sqrt(9.81 * 2 * pi / wave_length);
 float Qi = Q / w * A * 3.0;
 
 vec3 wave1;
 float amp1 = 0.045;
 vec3 dir1 = normalize(vec3(1.0, 1.0, 1.00));
 
 wave1.x = Qi * amp1 * dir1.x * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.y = Qi * amp1 * dir1.y * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.z = amp1 * sin(dot(w * dir1, position) + phase_constant * time);
 
 vec3 wave2;
 float amp2 = 0.015;
 vec3 dir2 = normalize(vec3(0.0, -0.2, -0.5));
 
 wave2.x = Qi * amp2 * dir1.x * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.y = Qi * amp2 * dir1.y * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.z = amp2 * sin(dot(w * dir2, position) + phase_constant * time);
 
 vec3 wave3;
 float amp3 = 0.015;
  vec3 dir3 = normalize(vec3(-0.45, -0.2, -0.15));
 
 wave3.x = Qi * amp3 * dir1.x * cos(dot(w * dir3, position) + phase_constant * time);
 wave3.y = Qi * amp3 * dir1.y * cos(dot(w * dir3, position) + phase_constant * time);
 wave3.z = amp3 * sin(dot(w * dir3, position) + phase_constant * time);
 
 fPosition += wave1 + wave2 + wave3;
 
 gl_Position = projection * view * model_matrix * vec4(fPosition, 1.0);
  
 vec3 diffuse = vec3(0.2, 0.6, 0.85);
 diffuse.gb += fPosition.z * 4.0;

  ourColor = diffuse;
}



































