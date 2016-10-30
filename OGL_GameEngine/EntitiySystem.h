#ifndef _ENTITY_SYSTEM_H_
#define _ENTITY_SYSTEM_H_
#include "EntityNodes.h"
#include "FPSCamera.h"

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
	std::vector<FPSCamera*> m_cameraList;
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
			for (std::vector<Mesh>::iterator it2 = (*it)->render->m_rcModel->m_meshes.begin(); it2 != (*it)->render->m_rcModel->m_meshes.end();)
			{
				//rendering
				glUseProgram((*it)->render->m_program);

				glUniformMatrix4fv((*it)->render->m_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr((*it)->transform->m_modelMatrix));
				glUniformMatrix4fv((*it)->render->m_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_viewMatrix));
				glUniformMatrix4fv((*it)->render->m_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_projMatrix));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "cameraPos"), 1, glm::value_ptr(m_cameraList[0]->GetPos()));
				
				for (int i = 0; i < m_plList.size(); i++)
				{
					std::string pLight = "pLight[";
					pLight += std::to_string(i);
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].position").c_str()), 1, glm::value_ptr(m_plList[i]->position));
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].ambient").c_str()), 1, glm::value_ptr(m_plList[i]->ambient));
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].diffuse").c_str()), 1, glm::value_ptr(m_plList[i]->diffuse));
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].specular").c_str()), 1, glm::value_ptr(m_plList[i]->specular));
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].constant").c_str()), m_plList[i]->constant);
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].linear").c_str()), m_plList[i]->linear);
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].quadratic").c_str()), m_plList[i]->quadratic);
				}

				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.direction"), 1, glm::value_ptr(m_dlList[0]->direction));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.ambient"), 1, glm::value_ptr(m_dlList[0]->ambient));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.diffuse"), 1, glm::value_ptr(m_dlList[0]->diffuse));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.specular"), 1, glm::value_ptr(m_dlList[0]->specular));

				glUniform1f(glGetUniformLocation((*it)->render->m_program, "material.shininess"), 16.0f);

				(*it2).Draw((*it)->render->m_program);

				glUseProgram(0);

				it2++;
			}

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

	void AddCamera(FPSCamera* in)
	{
		m_cameraList.push_back(in);
	}

	void RemoveCamera(FPSCamera* in)
	{
		for (std::vector<FPSCamera*>::iterator it = m_cameraList.begin(); it != m_cameraList.end();)
		{
			if ((*it) == in)
			{
				m_cameraList.erase(it);
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

			(*it)->transform->m_position += (*it)->movable->m_velocity * deltaT;

			it++;
		}
	}

};

class TransformSystem : public ISystem
{
private:
	std::vector<TransformNode*> m_tnList;

public:

	void AddNode(TransformNode* in)
	{
		m_tnList.push_back(in);
	}

	void RemoveNode(TransformNode* in)
	{
		for (std::vector<TransformNode*>::iterator it = m_tnList.begin(); it != m_tnList.end();)
		{
			if ((*it) == in)
			{
				m_tnList.erase(it);
				return;
			}
			++it;
		}

	}

	void Update(float deltaT)
	{
		for (std::vector<TransformNode*>::iterator it = m_tnList.begin(); it != m_tnList.end();)
		{

			//QUATERNIONS
			glm::quat quatX = glm::angleAxis((*it)->transform->m_rotAngles.x, (*it)->transform->m_quatRight);

			(*it)->transform->m_quatUp = quatX * (*it)->transform->m_quatUp;
			(*it)->transform->m_quatForward = quatX * (*it)->transform->m_quatForward;

			glm::quat quatY = glm::angleAxis((*it)->transform->m_rotAngles.y, (*it)->transform->m_quatUp);

			(*it)->transform->m_quatRight = quatY * (*it)->transform->m_quatRight;
			(*it)->transform->m_quatForward = quatY * (*it)->transform->m_quatForward;

			glm::quat quatZ = glm::angleAxis((*it)->transform->m_rotAngles.z, (*it)->transform->m_quatForward);

			(*it)->transform->m_quatRight = quatZ * (*it)->transform->m_quatRight;
			(*it)->transform->m_quatUp = quatZ * (*it)->transform->m_quatUp;

			//periodic re-ortho-normalization is needed to correct our axes

			(*it)->transform->m_quatUp = glm::cross((*it)->transform->m_quatRight, (*it)->transform->m_quatForward);
			(*it)->transform->m_quatRight = glm::cross((*it)->transform->m_quatForward, (*it)->transform->m_quatUp);

			//normalize

			(*it)->transform->m_quatRight = glm::normalize((*it)->transform->m_quatRight);
			(*it)->transform->m_quatUp = glm::normalize((*it)->transform->m_quatUp);
			(*it)->transform->m_quatForward = glm::normalize((*it)->transform->m_quatForward);

			(*it)->transform->m_quat = quatZ * quatY * quatX * (*it)->transform->m_quat;

			(*it)->transform->m_rotAngles = glm::vec3(0);

			(*it)->transform->m_modelMatrix = glm::translate(glm::mat4(1.0f), (*it)->transform->m_position) * glm::toMat4((*it)->transform->m_quat);

			it++;
		}
	}

	void UpdateForOne(TransformNode* _tn)
	{
		//QUATERNIONS
		glm::quat quatX = glm::angleAxis(_tn->transform->m_rotAngles.x, _tn->transform->m_quatRight);

		_tn->transform->m_quatUp = quatX * _tn->transform->m_quatUp;
		_tn->transform->m_quatForward = quatX * _tn->transform->m_quatForward;

		glm::quat quatY = glm::angleAxis(_tn->transform->m_rotAngles.y, _tn->transform->m_quatUp);

		_tn->transform->m_quatRight = quatY * _tn->transform->m_quatRight;
		_tn->transform->m_quatForward = quatY * _tn->transform->m_quatForward;

		glm::quat quatZ = glm::angleAxis(_tn->transform->m_rotAngles.z, _tn->transform->m_quatForward);

		_tn->transform->m_quatRight = quatZ * _tn->transform->m_quatRight;
		_tn->transform->m_quatUp = quatZ * _tn->transform->m_quatUp;

		//periodic re-ortho-normalization is needed to correct our axes

		_tn->transform->m_quatUp = glm::cross(_tn->transform->m_quatRight, _tn->transform->m_quatForward);
		_tn->transform->m_quatRight = glm::cross(_tn->transform->m_quatForward, _tn->transform->m_quatUp);

		//normalize

		_tn->transform->m_quatRight = glm::normalize(_tn->transform->m_quatRight);
		_tn->transform->m_quatUp = glm::normalize(_tn->transform->m_quatUp);
		_tn->transform->m_quatForward = glm::normalize(_tn->transform->m_quatForward);
		_tn->transform->m_quat = quatZ * quatY * quatX * _tn->transform->m_quat;
		
		_tn->transform->m_rotAngles = glm::vec3(0);
	}

};

#endif