#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform float time;
uniform vec3 normalMatrix;
uniform float speed;
uniform float wave_length;
uniform float steepness;

uniform vec3 direction_1;
uniform vec3 direction_2;
uniform vec3 direction_3;

out vec2 TexCoord;
out vec3 Normal;
out float relative_position;
out vec3 FragPos;

out VS_OUT {
vec3 FragPos;
vec2 TexCoords;
vec3 TangentLightPos;
vec3 TangentViewPos;
vec3 TangentFragPos;
}vs_out;

uniform vec3 lightPos;
uniform vec3 cameraPosition;

vec3 generateWave(float amp, vec3 direction, float num_waves, float steepness, float phase_constant, float w);

void main()
{
 vec3 fPosition = position;
 float pi = 3.1415f;
 float phase_constant = speed * 2.0 * pi / wave_length;
 float w = sqrt(9.81 * (2 * pi / wave_length));
 float num_waves = 3.0;
 
 float amp1 = 0.065;
 vec3 wave1 = generateWave(amp1, direction_1, num_waves, steepness, phase_constant, w);
 
 float amp2 = 0.015;
 vec3 wave2 = generateWave(amp2, direction_2, num_waves, steepness, phase_constant, w);
 
 float amp3 = 0.015;
 vec3 wave3 = generateWave(amp3, direction_3, num_waves, steepness, phase_constant, w);
 //fPosition += wave1 + wave2 + wave3;
 
 TexCoord = texCoord; // + time * speed * 0.35;
 
 gl_Position = projection * view * model_matrix * vec4(fPosition, 1.0);

 relative_position = (wave1.z / amp1 + wave2.z / amp2 + wave3.z / amp3) * 0.5 + 0.5;
  
 Normal = normal;
 
 vec3 T = normalize(vec3(model_matrix * vec4(tangent,   0.0)));
 vec3 N = normalize(vec3(model_matrix * vec4(normal,    0.0)));
 vec3 B = cross(N,T);
 
 mat3 TBN = transpose(mat3(T,B,N));
 vs_out.TangentLightPos = TBN * lightPos;
 vs_out.TangentViewPos = TBN * cameraPosition;
 vs_out.TangentFragPos = TBN * vec3(model_matrix * vec4(position, 1.0));
 
 FragPos = vec3(model_matrix * vec4(position, 1.0));
}

vec3 generateWave(float amp, vec3 direction, float num_waves, float steepness,
 float phase_constant, float w ) {
 vec3 dir = normalize(direction);
 float q = steepness / (w * amp * num_waves);
 
 vec3 wave;
 wave.x = q * amp * dir.x * cos(dot(w * dir, position) + phase_constant * time);
 wave.y = q * amp * dir.y * cos(dot(w * dir, position) + phase_constant * time);
 wave.z = amp * sin(dot(w * dir, position) + phase_constant * time);
 
 return wave;
}






















































