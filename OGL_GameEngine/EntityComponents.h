#ifndef _ENTITY_COMPONENT_H_
#define _ENTITY_COMPONENT_H_
#include "Headers.h"

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
		if (m_rcAsset->m_glVertBuffer)
		{
			glDeleteBuffers(1, &m_rcAsset->m_glIndexBuffer);
			glDeleteVertexArrays(1, &m_rcAsset->m_glVertArray);
			glDeleteBuffers(1, &m_rcAsset->m_glVertBuffer);
			m_rcAsset->m_glIndexBuffer = 0;
			m_rcAsset->m_glVertArray = 0;
			m_rcAsset->m_glVertBuffer = 0;
		}
	}

	GLuint m_program;
	GLint m_shaderModelMatLocation, m_shaderViewMatLocation, m_shaderProjMatLocation;
	glm::mat4* m_viewMatrix, * m_projMatrix;
	Asset* m_rcAsset;
};

class MovableComponent
{
public:
	MovableComponent() : m_acceleration(0.0f), m_velocity(0.0f){}

	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
};


#endif