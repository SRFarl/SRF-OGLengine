#version 430
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform vec3 cameraPos;

out VS_OUT {
out mat3 TBN;
out vec3 vNormalV;
out vec3 fragPos;
out vec2 outTexCoord;
out vec3 cameraPos;
} vs_out;

void main()
{
	mat4 modelView = viewMat * modelMat;
	
	vec3 T = normalize(vec3(modelMat * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(modelMat * vec4(normal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	
	//tangent space matrix. use this to convert from tangent space to world space
	vs_out.TBN = transpose(mat3(T, B, N));
	
	gl_Position = projMat * modelView * position;
	vs_out.fragPos = vec3(modelMat * position);
	vs_out.vNormalV = mat3(transpose(inverse(modelMat))) * normal;
	vs_out.cameraPos = cameraPos;
	vs_out.outTexCoord = texCoord;
}