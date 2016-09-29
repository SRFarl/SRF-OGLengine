#version 430 core

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
	sampler2D normal1;
    float shininess;
};  
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
in mat3 TBN;
in vec3 vNormalV;
in vec3 fragPos;
in vec2 outTexCoord;
in vec3 cameraPos;
} fs_in;

uniform Material material;
uniform PointLight light;
uniform DirectionalLight dLight;

out vec4 outputColor;
void main()
{
	vec3 outputColourV3 = vec3(0.0f, 0.0f, 0.0f);
	vec3 normal = texture(material.normal1, fs_in.outTexCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0) * fs_in.TBN;
	
	//point light 	
	vec3 lightDirPL = normalize((light.position) - fs_in.fragPos);
	
	//attenuation
	float distance = length((light.position) - fs_in.fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
	
	// Ambient
	vec3 ambientPL = light.ambient * vec3(texture(material.diffuse1, fs_in.outTexCoord)) * attenuation;
	//outputColourV3 += ambientPL;
	
	// Diffuse 
	float diffPL = max(dot(normal, lightDirPL), 0.0);
	vec3 diffusePL = light.diffuse * diffPL * vec3(texture(material.diffuse1, fs_in.outTexCoord)) * attenuation;  
	//outputColourV3 += diffusePL;
	
	// Specular
	vec3 viewDirPL = normalize(fs_in.cameraPos - fs_in.fragPos);
	vec3 reflectDirPL = reflect(-lightDirPL, normal);  
	float specPL = pow(max(dot(viewDirPL, reflectDirPL), 0.0), material.shininess);
	vec3 specularPL = light.specular * specPL * vec3(texture(material.specular1, fs_in.outTexCoord)) * attenuation;
	outputColourV3 += specularPL;
	
	//directional light
	vec3 lightDirDL = normalize(-dLight.direction);
	
	// Ambient
	vec3 ambientDL = dLight.ambient * vec3(texture(material.diffuse1, fs_in.outTexCoord));
	outputColourV3 += ambientDL;
	
	// Diffuse 
	float diffDL = max(dot(normal, lightDirDL), 0.0);
	vec3 diffuseDL = dLight.diffuse * diffDL * vec3(texture(material.diffuse1, fs_in.outTexCoord));
	outputColourV3 += diffuseDL;
	
	// Specular
	vec3 viewDirDL = normalize(fs_in.cameraPos - fs_in.fragPos);
	vec3 reflectDirDL = reflect(-lightDirDL, normal);  
	float specDL = pow(max(dot(viewDirDL, reflectDirDL), 0.0), material.shininess);
	vec3 specularDL = dLight.specular * specDL * vec3(texture(material.specular1, fs_in.outTexCoord));
	outputColourV3 += specularDL;
	
	outputColor = vec4(outputColourV3 , 1.0f);
}