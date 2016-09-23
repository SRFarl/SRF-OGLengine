#ifndef _ENTITY_COMPONENT_H_
#define _ENTITY_COMPONENT_H_
#include "Headers.h"

struct Material {
	GLuint diffuse;
	GLuint specular;
	GLfloat shininess;
};

struct PointLight {
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class RenderComponent
{
public:
	RenderComponent(GLuint _program, glm::mat4* viewMat, glm::mat4* projMat) : m_viewMatrix(viewMat), m_projMatrix(projMat)
	{
		m_program = _program;
		m_shaderModelMatLocation = glGetUniformLocation(_program, "modelMat");
		m_shaderViewMatLocation = glGetUniformLocation(_program, "viewMat");
		m_shaderProjMatLocation = glGetUniformLocation(_program, "projMat");
	}

	~RenderComponent()
	{
		if (m_glVertBuffer)
		{
			glDeleteBuffers(1, &m_glIndexBuffer);
			glDeleteVertexArrays(1, &m_glVertArray);
			glDeleteBuffers(1, &m_glVertBuffer);
			m_glIndexBuffer = 0;
			m_glVertArray = 0;
			m_glVertBuffer = 0;
		}
	}

	GLuint m_program;
	GLint m_shaderModelMatLocation, m_shaderViewMatLocation, m_shaderProjMatLocation;
	glm::mat4* m_viewMatrix, * m_projMatrix;
	GLuint m_glVertBuffer; //vertex VBO
	GLuint m_glNormBuffer; //normal VBO
	GLuint m_glUVBuffer; //uv VBO
	GLuint m_glIndexBuffer;
	GLuint m_glVertArray; //VAO
	Material m_glMaterial;
	GLsizei m_unVertexCount;
};

class MovableComponent
{
public:
	MovableComponent() : m_acceleration(0.0f), m_velocity(0.0f){}

	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
};


#endif