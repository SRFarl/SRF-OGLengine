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
in vec3 vertColour;
} fs_in;

uniform Material material;
uniform PointLight pLight[NR_POINT_LIGHTS];
uniform DirectionalLight dLight;
uniform bool diffuseMapping;
uniform bool normalMapping;
uniform bool specularMapping;
uniform float gamma;

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
	
	//gamma correction
	outputColor = pow(outputColor, vec4(1.0/gamma));
}

vec3 RenderDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 returnVec = vec3(0.0f);
	
	vec3 diffuseColour = vec3(0.0f);
	
	if(diffuseMapping)
	{
		//de-gamma the texture
		diffuseColour = vec3(pow(texture(material.diffuse1, fs_in.texCoord),vec4(gamma)));
	} else {
		diffuseColour = vec3(pow(fs_in.vertColour,vec3(gamma)));
	}
		
	//directional light
	vec3 lightDir = normalize(-light.direction);
	
	// Ambient
	vec3 ambient = light.ambient * diffuseColour;
	returnVec += ambient;
	
	// Diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColour;
	returnVec += diffuse;
	
	// Specular
	if(specularMapping)
	{
		vec3 reflectDir = reflect(-lightDir, normal);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * spec * vec3(texture(material.specular1, fs_in.texCoord));
		returnVec += specular;
	}
	
	return returnVec;
}

vec3 RenderPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 returnVec = vec3(0.0f);

	vec3 diffuseColour = vec3(0.0f);
	
	if(diffuseMapping)
	{
		//de-gamma the texture
		diffuseColour = vec3(pow(texture(material.diffuse1, fs_in.texCoord),vec4(gamma)));
	} else {
		diffuseColour = fs_in.vertColour;
	}
	
	//point light 	
	vec3 lightDir = normalize((light.position) - fs_in.fragPos);
	
	//attenuation
	float distance = length((light.position) - fs_in.fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
	
	// Ambient
	vec3 ambient = light.ambient * diffuseColour * attenuation;
	returnVec += ambient;
	
	// Diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColour * attenuation;  
	returnVec += diffuse;
	
	// Specular
	if(specularMapping)
	{
		vec3 reflectDir = reflect(-lightDir, normal);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * spec * vec3(texture(material.specular1, fs_in.texCoord)) * attenuation;
		returnVec += specular;
	}
	
	return returnVec;
}