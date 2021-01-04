#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 cameraPosition;
uniform float time;

uniform float contrast;
uniform float speed;
in float relative_position;
in vec3 Normal;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

vec2 rand2( vec2 coord ) {
  coord = mod(coord, 10000.0);
  return fract(sin(vec2(dot(coord,vec2(127.1,311.7)),dot(coord,vec2(269.5,183.3))))*43758.5453);
}

float cellular_noise(vec2 coord){
  vec2 i = floor(coord);
  vec2 f = fract(coord);
  
  float min_dist = 99999.0;
  
  for(float x = -1.0; x <= 1.0; x++)
  {
    for(float y = -1.0; y <= 1.0; y++)
    {
      vec2 node = rand2(i + vec2(x,y)) + vec2(x,y);
      float dist = sqrt((f-node).x * (f-node).x + (f - node).y * (f - node).y);
      
      min_dist = min(min_dist, dist);
    }  
  }
  
  return min_dist;
}

float fbm(vec2 coord) {
 int octaves = 4;
 
 float normalize_factor = 0.0;
 float value = 0.0;
 float scale = 0.5;
 
 for(int i = 0; i < octaves; i++){
  value += cellular_noise(coord) * scale;
  normalize_factor += scale;
  coord *= 2.0;
  scale *= 0.5;
 }
 
 return value / normalize_factor;
}

void main()
{
    vec2 coord = fs_in.FragPos.xz * 3.0;
    float value = fbm(coord);   
    
    vec3 normal = texture(normalMap, fs_in.TexCoords * 1.5).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    //diffuse color
    vec3 color = vec3(0.1, 0.5, 0.6);
    
    //ambient 
    float ambientStrength = 0.55;
    vec3 ambient = ambientStrength * color;
  	
    // diffuse 
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 specular = vec3(0.5) * spec;    
	FragColor = vec4(ambient + diffuse + specular, 1.0);// + value * relative_position * 0.2;
    //FragColor = vec4(normal, 1.0);
} 





































































