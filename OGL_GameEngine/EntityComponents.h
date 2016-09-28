#ifndef _ENTITY_COMPONENT_H_
#define _ENTITY_COMPONENT_H_
#include "Headers.h"
#include "Model.h"

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
	}

	GLuint m_program;
	GLint m_shaderModelMatLocation, m_shaderViewMatLocation, m_shaderProjMatLocation;
	glm::mat4* m_viewMatrix, * m_projMatrix;
	Model* m_rcModel;
};

class MovableComponent
{
public:
	MovableComponent() : m_acceleration(0.0f), m_velocity(0.0f){}

	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
};


#endif