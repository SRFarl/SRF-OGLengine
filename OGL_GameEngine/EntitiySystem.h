#ifndef _ENTITY_SYSTEM_H_
#define _ENTITY_SYSTEM_H_
#include "EntityNodes.h"
#include "FPSCamera.h"
#include "Math.h"

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
		glClearColor(0.1f, 0.1f, 0.05f, 0.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
					//If it's selected we want to highlight the mesh
					if ((*it)->selected != NULL && (*it)->selected->m_selected == true)
						glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].diffuse").c_str()), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
					else
						glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].diffuse").c_str()), 1, glm::value_ptr(m_plList[i]->diffuse));
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].specular").c_str()), 1, glm::value_ptr(m_plList[i]->specular));
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].constant").c_str()), m_plList[i]->constant);
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].linear").c_str()), m_plList[i]->linear);
					glUniform1f(glGetUniformLocation((*it)->render->m_program, (pLight + "].quadratic").c_str()), m_plList[i]->quadratic);
				}

				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.direction"), 1, glm::value_ptr(m_dlList[0]->direction));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.ambient"), 1, glm::value_ptr(m_dlList[0]->ambient));

				//If it's selected we want to highlight the mesh
				if ((*it)->selected != NULL && (*it)->selected->m_selected == true)
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
				else
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.diffuse"), 1, glm::value_ptr(m_dlList[0]->diffuse));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "dLight.specular"), 1, glm::value_ptr(m_dlList[0]->specular));

				glUniform1f(glGetUniformLocation((*it)->render->m_program, "material.shininess"), 16.0f);

				glUniform1f(glGetUniformLocation((*it)->render->m_program, "gamma"), GAMMA_CORRECTION);

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
			if (!(*it)->movable->m_skipThisFrame)
			{
				(*it)->movable->m_velocity = (*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT);

				(*it)->transform->m_position += (*it)->movable->m_velocity * deltaT;

				(*it)->movable->m_acceleration = glm::vec3(0.0f);
			}
			else
			{
				(*it)->movable->m_acceleration = glm::vec3(0.0f);
				(*it)->movable->m_skipThisFrame = false;
			}

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
			if (!(*it)->transform->m_isStatic)
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

				glm::mat4 wut = glm::translate(glm::mat4(1.0f), (*it)->transform->m_position);

				(*it)->transform->m_modelMatrix = glm::translate(glm::mat4(1.0f), (*it)->transform->m_position) * glm::toMat4((*it)->transform->m_quat);
			}

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

		_tn->transform->m_modelMatrix = glm::translate(glm::mat4(1.0f), _tn->transform->m_position) * glm::toMat4(_tn->transform->m_quat);
	}

};

class CollisionSystem : public ISystem
{
private:
	std::vector<SphereCollisionNode*> m_scList;
	std::vector<AABBCollisionNode*> m_aabbList;

public:

	void AddSphereCollisionNode(SphereCollisionNode* in)
	{
		m_scList.push_back(in);
	}

	void RemoveSphereCollisionNode(SphereCollisionNode* in)
	{
		for (std::vector<SphereCollisionNode*>::iterator it = m_scList.begin(); it != m_scList.end();)
		{
			if ((*it) == in)
			{
				m_scList.erase(it);
				return;
			}
			++it;
		}
	}

	void AddAABBCollisionNode(AABBCollisionNode* in)
	{
		m_aabbList.push_back(in);
	}

	void RemoveAABBCollisionNode(AABBCollisionNode* in)
	{
		for (std::vector<AABBCollisionNode*>::iterator it = m_aabbList.begin(); it != m_aabbList.end();)
		{
			if ((*it) == in)
			{
				m_aabbList.erase(it);
				return;
			}
			++it;
		}
	}

	void Update(float deltaT)
	{
		//SPHERE
		for (std::vector<SphereCollisionNode*>::iterator it = m_scList.begin(); it != m_scList.end();)
		{
			if (!(*it)->transform->m_isStatic && (*it)->movable->m_velocity != glm::vec3(0))
			{
				//this sphere is not a static sphere therefore leave it to the non-static objects to calculate collision
				for (std::vector<SphereCollisionNode*>::iterator it2 = m_scList.begin(); it2 != m_scList.end();)
				{
					if (it2 != it)
					{
						//don't check for collision against itself
						if ((*it2)->transform->m_isStatic || (*it2)->movable == NULL)
						{
							//kinematic-static collision
							glm::vec3 timestepVelocity = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;

							float magnitudeAlongBeforeCollsion = Math::KinematicSphereStaticSphereCollision(
								(*it)->transform->m_position + (*it)->sCollision->m_centrePointOffset,		//CenterA
								timestepVelocity,															//DirectionA
								(*it)->sCollision->m_radius,												//RadiusA
								(*it2)->transform->m_position + (*it2)->sCollision->m_centrePointOffset,	//CenterB
								(*it2)->sCollision->m_radius												//RadiusB
								);
							if (magnitudeAlongBeforeCollsion > 0)
							{
								//the spheres will touch
								//find normal of collision point on sphereB
								glm::vec3 bNormal = glm::normalize(
									(((*it)->transform->m_position + (*it)->sCollision->m_centrePointOffset) + (glm::normalize(timestepVelocity) * magnitudeAlongBeforeCollsion))
									- ((*it2)->transform->m_position + (*it2)->sCollision->m_centrePointOffset)
									);

								float scalar = glm::dot(-bNormal, glm::normalize(timestepVelocity));
								float lengthOfDirectionAfterCollision = (glm::length(timestepVelocity) - magnitudeAlongBeforeCollsion);

								(*it)->transform->m_position = (*it)->transform->m_position + ((timestepVelocity) + (lengthOfDirectionAfterCollision * bNormal * scalar * 2.0f));
								(*it)->movable->m_velocity = ((*it)->movable->m_velocity - (2 * glm::dot((*it)->movable->m_velocity, bNormal) * bNormal)) * (*it2)->sCollision->m_damping;
								(*it)->movable->m_skipThisFrame = true;
							}
						}
						else
						{
							//kinematic-kinematic collision
							glm::vec3 timestepVelocityA = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;
							glm::vec3 timestepVelocityB = ((*it2)->movable->m_velocity + ((*it2)->movable->m_acceleration * deltaT)) * deltaT;

							glm::vec3 resultA;
							glm::vec3 resultB;

							if (Math::KinematicSphereKinematicSphereCollision(
								(*it)->transform->m_position + (*it)->sCollision->m_centrePointOffset,		//CenterA
								timestepVelocityA,															//DirectionA
								(*it)->sCollision->m_radius,												//RadiusA
								(*it)->movable->m_mass,														//MassA
								(*it2)->transform->m_position + (*it2)->sCollision->m_centrePointOffset,	//CenterB
								timestepVelocityB,															//DirectionB
								(*it2)->sCollision->m_radius,												//RadiusB
								(*it2)->movable->m_mass,													//MassB
								resultA,																	//ResultA
								resultB																		//ResultB
								))
							{
								(*it)->movable->m_velocity = resultA / deltaT;
								(*it2)->movable->m_velocity = resultB / deltaT;
							}
						}
					}
					it2++;
				}
			}
			it++;
		}

		//AABB
		for (std::vector<AABBCollisionNode*>::iterator it = m_aabbList.begin(); it != m_aabbList.end();)
		{
			if (!(*it)->transform->m_isStatic && (*it)->movable->m_velocity != glm::vec3(0))
			{
				//this AABB is not a static sphere therefore leave it to the non-static objects to calculate collision
				for (std::vector<AABBCollisionNode*>::iterator it2 = m_aabbList.begin(); it2 != m_aabbList.end();)
				{
					if (it2 != it)
					{
						//don't check for collision against itself
						if ((*it2)->transform->m_isStatic || (*it2)->movable == NULL)
						{
							glm::vec3 timestepVelocityA = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;

							//kinematic-static collision
							glm::vec3 normal;

							float collisiontime = Math::KinematicAABBStaticAABB(
								(*it)->transform->m_position + (*it)->aabbCollision->m_minOffset,		//MinA
								(*it)->transform->m_position + (*it)->aabbCollision->m_maxOffset,		//MinA
								timestepVelocityA,														//DirectionA
								(*it2)->transform->m_position + (*it2)->aabbCollision->m_minOffset,		//MinA
								(*it2)->transform->m_position + (*it2)->aabbCollision->m_maxOffset,		//MinA
								normal
								);

							if (collisiontime > 0)
							{
								//apply velocity until hit
								(*it)->transform->m_position += timestepVelocityA * collisiontime;

								//deflect remaining
								if (abs(normal.x) > 0.0001f)
									(*it)->movable->m_velocity.x = -((*it)->movable->m_velocity.x + ((*it)->movable->m_acceleration.x * deltaT)) * (*it2)->aabbCollision->m_damping;
								if (abs(normal.y) > 0.0001f)
									(*it)->movable->m_velocity.y = -((*it)->movable->m_velocity.y + ((*it)->movable->m_acceleration.y * deltaT)) * (*it2)->aabbCollision->m_damping;
								if (abs(normal.z) > 0.0001f)
									(*it)->movable->m_velocity.z = -((*it)->movable->m_velocity.z + ((*it)->movable->m_acceleration.z * deltaT)) * (*it2)->aabbCollision->m_damping;

								glm::vec3 meme = ((*it)->movable->m_velocity) * deltaT;

								(*it)->transform->m_position += ((*it)->movable->m_velocity) * deltaT * (1.0f - collisiontime);
								(*it)->movable->m_skipThisFrame = true;
							}
						}
						else
						{
							//kinematic-kinematic collision
							glm::vec3 timestepVelocityA = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;
							glm::vec3 timestepVelocityB = ((*it2)->movable->m_velocity + ((*it2)->movable->m_acceleration * deltaT)) * deltaT;

							glm::vec3 resultA;
							glm::vec3 resultB;

							if (Math::KinematicAABBKinematicAABBCollision(
								(*it)->transform->m_position + (*it)->aabbCollision->m_midOffset,			//CenterA
								timestepVelocityA,															//DirectionA
								(*it)->transform->m_position + (*it)->aabbCollision->m_minOffset,			//MinA
								(*it)->transform->m_position + (*it)->aabbCollision->m_maxOffset,			//MaxA
								(*it)->movable->m_mass,														//MassA
								(*it2)->transform->m_position + (*it2)->aabbCollision->m_midOffset,			//CenterB
								timestepVelocityB,															//DirectionB
								(*it2)->transform->m_position + (*it2)->aabbCollision->m_minOffset,			//MinA
								(*it2)->transform->m_position + (*it2)->aabbCollision->m_maxOffset,			//MaxA
								(*it2)->movable->m_mass,													//MassB
								resultA,																	//ResultA
								resultB																		//ResultB
								))
							{
								(*it)->movable->m_velocity = resultA / deltaT;
								(*it2)->movable->m_velocity = resultB / deltaT;
							}
						}
					}
					it2++;
				}
			}
			it++;
		}
	}
};


#endif