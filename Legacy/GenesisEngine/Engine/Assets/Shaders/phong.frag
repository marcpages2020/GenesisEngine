#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec4 diffuseColor;
uniform vec3 cameraPosition;
uniform sampler2D diffuseMap;
uniform float time;
uniform bool hasDiffuseMap;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  

Material material;

void main()
{
    material.ambient = vec3(0.5, 0.5, 0.5);
  	material.diffuse = vec3(0.5, 0.5, 0.5);
    material.specular = vec3(0.5, 0.5, 0.5);

    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    vec3 viewDir = normalize(cameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
   
   if(hasDiffuseMap) {
     color = diffuseColor * texture(diffuseMap, TexCoord);
    }
    else {
     color = diffuseColor;
    }
    
   color.rgb += diffuse + specular;
}
















