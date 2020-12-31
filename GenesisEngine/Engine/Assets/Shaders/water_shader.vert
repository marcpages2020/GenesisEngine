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

uniform float time;

uniform float speed;
uniform float wave_length;
uniform float steepness;

uniform vec3 direction_1;
uniform vec3 direction_2;
uniform vec3 direction_3;

out float relative_position;

void main()
{
 vec3 fPosition = position;
 float pi = 3.1415f;
 float phase_constant = speed * 2.0 * pi / wave_length;
 float w = sqrt(9.81 * (2 * pi / wave_length));
 float num_waves = 3.0;
 
 vec3 wave1;
 float amp1 = 0.05;
 vec3 dir1 = normalize(direction_1);
 float q1 = steepness / (w * amp1 * num_waves);
 wave1.x = q1 * amp1 * dir1.x * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.y = q1 * amp1 * dir1.y * cos(dot(w * dir1, position) + phase_constant * time);
 wave1.z = amp1 * sin(dot(w * dir1, position) + phase_constant * time);
 
 vec3 wave2;
 float amp2 = 0.015;
 vec3 dir2 = normalize(direction_2);
  float q2 = steepness / (w * amp2 * num_waves);
  
 wave2.x = q2 * amp2 * dir1.x * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.y = q2 * amp2 * dir1.y * cos(dot(w * dir2, position) + phase_constant * time);
 wave2.z = amp2 * sin(dot(w * dir2, position) + phase_constant * time);
 
 vec3 wave3;
 float amp3 = 0.015;
 vec3 dir3 = normalize(direction_3);
 float q3 = steepness / (w * amp3 * num_waves);
  
 wave3.x = q3 * amp3 * dir1.x * cos(dot(w * dir3, position) + phase_constant * time);
 wave3.y = q3 * amp3 * dir1.y * cos(dot(w * dir3, position) + phase_constant * time);
 wave3.z = amp3 * sin(dot(w * dir3, position) + phase_constant * time);
 
 fPosition += wave1 + wave2 + wave3;
 TexCoord = texCoord + time * speed * 0.35;
 
 gl_Position = projection * view * model_matrix * vec4(fPosition, 1.0);
  
 vec3 diffuse = vec3(0.2, 0.6, 0.85);

  ourColor = diffuse;
  relative_position = (wave1.z / amp1 + wave2.z / amp2 + wave3.z / amp3) * 0.5 + 0.5;
}












































