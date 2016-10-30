#ifndef _ENTITY_COMPONENT_H_
#define _ENTITY_COMPONENT_H_
#include "Headers.h"
#include "Model.h"

class RenderComponent
{
public:
	RenderComponent(GLuint _program, glm::mat4* viewMat, glm::mat4* projMat) : 
		m_viewMatrix(viewMat), 
		m_projMatrix(projMat)
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
	MovableComponent() : 
		m_acceleration(0.0f),
		m_velocity(0.0f)
	{}

	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
};

class TransformComponent
{
public:
	TransformComponent(glm::vec3 _initPos, glm::vec3 _initRot) :
		m_position(_initPos),
		m_rotAngles(_initRot),
		m_quatRight(1.0f, 0.f, 0.f),
		m_quatUp(0.f, 1.0f, 0.f),
		m_quatForward(0.f, 0.f, 1.0f)
	{}
	glm::vec3 m_position;

	glm::mat4 m_modelMatrix;

	glm::quat m_quat;
	glm::vec3 m_rotAngles;
	glm::vec3 m_quatRight;
	glm::vec3 m_quatUp;
	glm::vec3 m_quatForward;
};
#endif