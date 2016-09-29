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


#define NR_POINT_LIGHTS 1

in VS_OUT {
in mat3 TBN;
in vec3 vNormalV;
in vec3 fragPos;
in vec2 texCoord;
in vec3 cameraPos;
} fs_in;

uniform Material material;
uniform PointLight pLight[NR_POINT_LIGHTS];
uniform DirectionalLight dLight;
uniform bool normalMapping;

out vec4 outputColor;

vec3 RenderPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 RenderDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 outputColourV3 = vec3(0.0f, 0.0f, 0.0f);
	vec3 normal;
	if(normalMapping)
	{
		normal = texture(material.normal1, fs_in.texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0) * fs_in.TBN;
	} else {
		normal = fs_in.vNormalV;
	}
	
	vec3 viewDir = normalize(fs_in.cameraPos - fs_in.fragPos);
	
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		outputColourV3 += RenderPointLight(pLight[i], normal, viewDir);
	
	outputColourV3 += RenderDirectionalLight(dLight, normal, viewDir);
	
	outputColor = vec4(outputColourV3 , 1.0f);
}

vec3 RenderDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	//directional light
	vec3 lightDir = normalize(-light.direction);
	
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, fs_in.texCoord));
	
	// Diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texCoord));
	
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, fs_in.texCoord));
	
	return vec3(ambient + diffuse + specular);
}

vec3 RenderPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	//point light 	
	vec3 lightDir = normalize((light.position) - fs_in.fragPos);
	
	//attenuation
	float distance = length((light.position) - fs_in.fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
	
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse1, fs_in.texCoord)) * attenuation;
	
	// Diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse1, fs_in.texCoord)) * attenuation;  

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular1, fs_in.texCoord)) * attenuation;
	
	return vec3(ambient + diffuse + specular);
}