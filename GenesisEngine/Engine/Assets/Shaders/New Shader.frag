#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec3 cameraPosition;
uniform bool hasDiffuseMap;

struct Material {
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

Light light;

Material material;

void main()
{
	material.ambient = vec3(0.35, 0.35, 0.35);
	material.diffuse = vec3(0.75, 0.75, 0.75);
	material.specular = vec3(0.65, 0.65, 0.65);

	light.ambient = vec3(0.2, 0.2, 0.2);
	light.position = vec3(0.0, 10.0, 0.0);
	light.diffuse = vec3(0.5, 0.5, 0.5);
	light.specular = vec3(0.2, 0.2, 0.2);

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

	color = vec4(material.ambient + diffuse + specular, 1.0);
}
