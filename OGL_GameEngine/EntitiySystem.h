#ifndef _ENTITY_SYSTEM_H_
#define _ENTITY_SYSTEM_H_
#include "EntityNodes.h"

class ISystem
{
public:
	virtual void Update(float deltaT) = 0;
};
//move system

//render system
class RenderSystem : public ISystem
{
private:
	std::vector<RenderNode*> m_rnList;
	std::vector<PointLight*> m_plList;
	std::vector<DirectionalLight*> m_dlList;

public:

	void AddNode(RenderNode* in)
	{
		m_rnList.push_back(in);
	}

	void RemoveNode(RenderNode* in)
	{
		for (std::vector<RenderNode*>::iterator it = m_rnList.begin(); it != m_rnList.end();)
		{
			if ((*it) == in)
			{
				m_rnList.erase(it);
				return;
			}
			++it;
		}

	}

	void Update(float deltaT)
	{
		for (std::vector<RenderNode*>::iterator it = m_rnList.begin(); it != m_rnList.end();)
		{

			glUseProgram((*it)->render->m_program);

			glUniformMatrix4fv((*it)->render->m_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->position));
			glUniformMatrix4fv((*it)->render->m_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_viewMatrix));
			glUniformMatrix4fv((*it)->render->m_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_projMatrix));

			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "light.position"), 1, glm::value_ptr(m_plList[0]->position));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "light.ambient"), 1, glm::value_ptr(m_plList[0]->ambient));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "light.diffuse"), 1, glm::value_ptr(m_plList[0]->diffuse));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "light.specular"), 1, glm::value_ptr(m_plList[0]->specular));
			glUniform1f(glGetUniformLocation((*it)->render->m_program, "light.constant"), m_plList[0]->constant);
			glUniform1f(glGetUniformLocation((*it)->render->m_program, "light.linear"), m_plList[0]->linear);
			glUniform1f(glGetUniformLocation((*it)->render->m_program, "light.quadratic"), m_plList[0]->quadratic);

			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.direction"), 1, glm::value_ptr(m_dlList[0]->direction));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.ambient"), 1, glm::value_ptr(m_dlList[0]->ambient));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.diffuse"), 1, glm::value_ptr(m_dlList[0]->diffuse));
			glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.specular"), 1, glm::value_ptr(m_dlList[0]->specular));

			glUniform1f(glGetUniformLocation((*it)->render->m_program, "material.shininess"), (*it)->render->m_glMaterial.shininess);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (*it)->render->m_glMaterial.diffuse);
			glUniform1i(glGetUniformLocation((*it)->render->m_program, "material.diffuse"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, (*it)->render->m_glMaterial.specular);
			glUniform1i(glGetUniformLocation((*it)->render->m_program, "material.specular"), 1);

			glBindVertexArray((*it)->render->m_glVertArray);

			glDrawArrays(GL_TRIANGLES, 0, (*it)->render->m_unVertexCount);

			glBindVertexArray(0);

			glUseProgram(0);

			it++;
		}
	}

	void AddPointLight(PointLight* in)
	{
		m_plList.push_back(in);
	}

	void RemovePointLight(PointLight* in)
	{
		for (std::vector<PointLight*>::iterator it = m_plList.begin(); it != m_plList.end();)
		{
			if ((*it) == in)
			{
				m_plList.erase(it);
				return;
			}
			++it;
		}

	}

	void AddDirectionalLight(DirectionalLight* in)
	{
		m_dlList.push_back(in);
	}

	void RemoveDirectionalLight(DirectionalLight* in)
	{
		for (std::vector<DirectionalLight*>::iterator it = m_dlList.begin(); it != m_dlList.end();)
		{
			if ((*it) == in)
			{
				m_dlList.erase(it);
				return;
			}
			++it;
		}

	}

};

class MovableSystem : public ISystem
{
private:
	std::vector<MovableNode*> m_mnList;

public:

	void AddNode(MovableNode* in)
	{
		m_mnList.push_back(in);
	}

	void RemoveNode(MovableNode* in)
	{
		for (std::vector<MovableNode*>::iterator it = m_mnList.begin(); it != m_mnList.end();)
		{
			if ((*it) == in)
			{
				m_mnList.erase(it);
				return;
			}
			++it;
		}

	}

	void Update(float deltaT)
	{
		for (std::vector<MovableNode*>::iterator it = m_mnList.begin(); it != m_mnList.end();)
		{

			(*it)->movable->m_velocity = (*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT);

			*(*it)->position += (*it)->movable->m_velocity * deltaT;

			it++;
		}
	}

};

#endif