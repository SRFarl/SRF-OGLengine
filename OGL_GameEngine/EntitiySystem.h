#ifndef _ENTITY_SYSTEM_H_
#define _ENTITY_SYSTEM_H_
#include "EntityNodes.h"
#include "Camera.h"
#include "Math.h"
#include "flexutil.h"
#include "SkyBox.h"

//systems only hold references to nodes and logic

class ISystem
{
public:
	virtual void Update(float deltaT) = 0;
};

class RenderSystem : public ISystem
{
private:
	std::vector<std::shared_ptr<Camera>> m_cameraList;		//list of cameras
	std::vector<std::shared_ptr<RenderNode>> m_rnList;			//list of render nodes
	std::vector<std::shared_ptr<PointLight>> m_plList;			//list of point lights
	std::vector<std::shared_ptr<DirectionalLight>> m_dlList;	//list of directional lights
	std::shared_ptr<SkyBox> m_skyBox;

public:

	void AddNode(std::shared_ptr<RenderNode> in)
	{
		//adds a render node to the system
		m_rnList.push_back(in);
	}

	void RemoveNode(std::shared_ptr<RenderNode> in)
	{
		//removes a render node from the system
		for (std::vector<std::shared_ptr<RenderNode>>::iterator it = m_rnList.begin(); it != m_rnList.end();)
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
		//The render loop
		//Clear
		glClearColor(0.1f, 0.1f, 0.05f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render each render node
		for (std::vector<std::shared_ptr<RenderNode>>::iterator it = m_rnList.begin(); it != m_rnList.end();)
		{
			//render every mesh of the model
			for (std::vector<ModelMesh>::iterator it2 = (*it)->render->m_rcModel->m_meshes.begin(); it2 != (*it)->render->m_rcModel->m_meshes.end();)
			{
				//set the gl program
				glUseProgram((*it)->render->m_program);

				//apply matrix uniforms
				glUniformMatrix4fv((*it)->render->m_shaderModelMatLocation, 1, GL_FALSE, glm::value_ptr((*it)->transform->m_modelMatrix));
				glUniformMatrix4fv((*it)->render->m_shaderViewMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_viewMatrix));
				glUniformMatrix4fv((*it)->render->m_shaderProjMatLocation, 1, GL_FALSE, glm::value_ptr(*(*it)->render->m_projMatrix));
				glUniform3fv(glGetUniformLocation((*it)->render->m_program, "cameraPos"), 1, glm::value_ptr(m_cameraList[0]->GetPos()));
				
				//apply light uniforms
				for (int i = 0; i < m_plList.size(); i++)
				{
					std::string pLight = "pLight[";
					pLight += std::to_string(i);
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].position").c_str()), 1, glm::value_ptr(m_plList[i]->position));
					glUniform3fv(glGetUniformLocation((*it)->render->m_program, (pLight + "].ambient").c_str()), 1, glm::value_ptr(m_plList[i]->ambient));
					//If it's selected we want to highlight the mesh
					if ((*it)->selected != NULL && (*it)->selected->m_selected == true)
						//change colour of lightning to red
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

				//other uniforms
				glUniform1f(glGetUniformLocation((*it)->render->m_program, "material.shininess"), 16.0f);
				glUniform1f(glGetUniformLocation((*it)->render->m_program, "gamma"), GAMMA_CORRECTION);

				(*it2).Draw((*it)->render->m_program);
				
				//reset the gl program
				glUseProgram(0);

				it2++;
			}

			it++;
		}

		//render skybox
		if (m_skyBox != NULL)
		{
			// Draw skybox as last
			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			m_skyBox->m_skyboxShader->UseShader();
			glUniformMatrix4fv(glGetUniformLocation(m_skyBox->m_skyboxShader->getShaderProgram(), "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(*m_cameraList[0]->GetView()))));
			glUniformMatrix4fv(glGetUniformLocation(m_skyBox->m_skyboxShader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(*m_cameraList[0]->GetProj()));
			// skybox cube
			glBindVertexArray(m_skyBox->m_skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(m_skyBox->m_skyboxShader->getShaderProgram(), "skybox"), 0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->m_cubemapTexId);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // Set depth function back to default 
		}
	}

	void AddPointLight(std::shared_ptr<PointLight> in)
	{
		//adds a point light to the system
		m_plList.push_back(in);
	}

	void RemovePointLight(std::shared_ptr<PointLight> in)
	{
		//removes a point light from the system
		for (std::vector<std::shared_ptr<PointLight>>::iterator it = m_plList.begin(); it != m_plList.end();)
		{
			if ((*it) == in)
			{
				m_plList.erase(it);
				return;
			}
			++it;
		}

	}

	void AddDirectionalLight(std::shared_ptr<DirectionalLight> in)
	{
		//adds a directional light to the system
		m_dlList.push_back(in);
	}

	void RemoveDirectionalLight(std::shared_ptr<DirectionalLight> in)
	{
		//removes a directional light from the system
		for (std::vector<std::shared_ptr<DirectionalLight>>::iterator it = m_dlList.begin(); it != m_dlList.end();)
		{
			if ((*it) == in)
			{
				m_dlList.erase(it);
				return;
			}
			++it;
		}

	}

	void AddCamera(std::shared_ptr<Camera> in)
	{
		//adds a camera to the system
		m_cameraList.push_back(in);
	}

	void RemoveCamera(std::shared_ptr<Camera> in)
	{
		//removes a camera from the system
		for (std::vector<std::shared_ptr<Camera>>::iterator it = m_cameraList.begin(); it != m_cameraList.end();)
		{
			if ((*it) == in)
			{
				m_cameraList.erase(it);
				return;
			}
			++it;
		}
	}

	void AddSkyBox(std::shared_ptr<SkyBox> in)
	{
		m_skyBox = in;
	}

	void RemoveSkyBox()
	{
		m_skyBox = NULL;
	}

};

class MovableSystem : public ISystem
{
private:
	std::vector<std::shared_ptr<MovableNode>> m_mnList;

public:

	void AddNode(std::shared_ptr<MovableNode> in)
	{
		//add a node to the system
		m_mnList.push_back(in);
	}

	void RemoveNode(std::shared_ptr<MovableNode> in)
	{
		//remove a node from the system
		for (std::vector<std::shared_ptr<MovableNode>>::iterator it = m_mnList.begin(); it != m_mnList.end();)
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
		//movable system update lop
		for (std::vector<std::shared_ptr<MovableNode>>::iterator it = m_mnList.begin(); it != m_mnList.end();)
		{
			//iterate over all the nodes
			if (!(*it)->movable->m_skipThisFrame)
			{
				//update velocity
				(*it)->movable->m_velocity = (*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT);

				//apply velocity to position
				(*it)->transform->m_position += (*it)->movable->m_velocity * deltaT;

				//reset acceleration
				(*it)->movable->m_acceleration = glm::vec3(0.0f);
			}
			else
			{
				//zero acceleration
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
	std::vector<std::shared_ptr<TransformNode>> m_tnList;

public:

	void AddNode(std::shared_ptr<TransformNode> in)
	{
		//add a node to the system
		m_tnList.push_back(in);
	}

	void RemoveNode(std::shared_ptr<TransformNode> in)
	{
		//remove a node from the system
		for (std::vector<std::shared_ptr<TransformNode>>::iterator it = m_tnList.begin(); it != m_tnList.end();)
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
		//update loop for the transform system
		for (std::vector<std::shared_ptr<TransformNode>>::iterator it = m_tnList.begin(); it != m_tnList.end();)
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

				//build final quat
				(*it)->transform->m_quat = quatZ * quatY * quatX * (*it)->transform->m_quat;

				//zero rotations
				(*it)->transform->m_rotAngles = glm::vec3(0);

				//build model matrix
				(*it)->transform->m_modelMatrix = glm::translate(glm::mat4(1.0f), (*it)->transform->m_position) * glm::toMat4((*it)->transform->m_quat);
			}

			it++;
		}
	}

	void UpdateForOne(std::shared_ptr<TransformNode> _tn)
	{
		//used outside of the update loop
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

		//build final quat
		_tn->transform->m_quat = quatZ * quatY * quatX * _tn->transform->m_quat;
		
		//zero rotations
		_tn->transform->m_rotAngles = glm::vec3(0);

		//build model matrix
		_tn->transform->m_modelMatrix = glm::translate(glm::mat4(1.0f), _tn->transform->m_position) * glm::toMat4(_tn->transform->m_quat);
	}

};

class CollisionSystem : public ISystem
{
private:
	std::vector<std::shared_ptr<SphereCollisionNode>> m_scList;
	std::vector<std::shared_ptr<AABBCollisionNode>> m_aabbList;

public:

	void AddSphereCollisionNode(std::shared_ptr<SphereCollisionNode> in)
	{
		//add a sphere collision node to the system
		m_scList.push_back(in);
	}

	void RemoveSphereCollisionNode(std::shared_ptr<SphereCollisionNode> in)
	{
		//remove a sphere collision node to the system
		for (std::vector<std::shared_ptr<SphereCollisionNode>>::iterator it = m_scList.begin(); it != m_scList.end();)
		{
			if ((*it) == in)
			{
				m_scList.erase(it);
				return;
			}
			++it;
		}
	}

	void AddAABBCollisionNode(std::shared_ptr<AABBCollisionNode> in)
	{
		//add a aabb collision node to the system
		m_aabbList.push_back(in);
	}

	void RemoveAABBCollisionNode(std::shared_ptr<AABBCollisionNode> in)
	{
		//remove a aabb collision node to the system
		for (std::vector<std::shared_ptr<AABBCollisionNode>>::iterator it = m_aabbList.begin(); it != m_aabbList.end();)
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
		//update loop for the collision system
		//SPHERE
		for (std::vector<std::shared_ptr<SphereCollisionNode>>::iterator it = m_scList.begin(); it != m_scList.end();)
		{
			//loop over every sphere
			if (!(*it)->transform->m_isStatic && (*it)->movable->m_velocity != glm::vec3(0))
			{
				//this sphere is not a static sphere therefore leave it to the non-static objects to calculate collision
				for (std::vector<std::shared_ptr<SphereCollisionNode>>::iterator it2 = m_scList.begin(); it2 != m_scList.end();)
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

								//scalar between 1 and 0 of how inline the normal is to direction
								float scalar = glm::dot(-bNormal, glm::normalize(timestepVelocity));
								float lengthOfDirectionAfterCollision = (glm::length(timestepVelocity) - magnitudeAlongBeforeCollsion);

								//set the position and velocity
								//it work better if we don't apply the position and just leave it to transform system to apply velocity.
								//we zero acceleration because it can cause objects to fall through each other
								//(*it)->transform->m_position = (*it)->transform->m_position + ((timestepVelocity) + (lengthOfDirectionAfterCollision * bNormal * scalar * 2.0f));
								(*it)->movable->m_velocity = ((*it)->movable->m_velocity - (2 * glm::dot((*it)->movable->m_velocity, bNormal) * bNormal)) * (*it2)->sCollision->m_damping;
								(*it)->movable->m_acceleration = glm::vec3(0);
								//(*it)->movable->m_skipThisFrame = true;
							}
						}
						else
						{
							//kinematic-kinematic collision
							glm::vec3 timestepVelocityA = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;
							glm::vec3 timestepVelocityB = ((*it2)->movable->m_velocity + ((*it2)->movable->m_acceleration * deltaT)) * deltaT;

							//resulting velocitys
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
								//apply resulting velocity
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
		for (std::vector<std::shared_ptr<AABBCollisionNode>>::iterator it = m_aabbList.begin(); it != m_aabbList.end();)
		{
			//loop over every aabb
			if (!(*it)->transform->m_isStatic && (*it)->movable->m_velocity != glm::vec3(0))
			{
				//this AABB is not a static sphere therefore leave it to the non-static objects to calculate collision
				for (std::vector<std::shared_ptr<AABBCollisionNode>>::iterator it2 = m_aabbList.begin(); it2 != m_aabbList.end();)
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

								//apply position
								(*it)->transform->m_position += ((*it)->movable->m_velocity) * deltaT * (1.0f - collisiontime);
								(*it)->movable->m_skipThisFrame = true;
							}
						}
						else
						{
							//kinematic-kinematic collision
							glm::vec3 timestepVelocityA = ((*it)->movable->m_velocity + ((*it)->movable->m_acceleration * deltaT)) * deltaT;
							glm::vec3 timestepVelocityB = ((*it2)->movable->m_velocity + ((*it2)->movable->m_acceleration * deltaT)) * deltaT;

							//resulting velocitys
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

class FlexRigidSystem : public ISystem
{
private:
	std::vector <std::shared_ptr<FlexRigidNode>> m_frList;

public:

	void AddNode(std::shared_ptr<FlexRigidNode> in)
	{
		//add a node to the system
		m_frList.push_back(in);
	}

	void RemoveNode(std::shared_ptr<FlexRigidNode> in)
	{
		//remove a node from the system
		for (std::vector<std::shared_ptr<FlexRigidNode>>::iterator it = m_frList.begin(); it != m_frList.end();)
		{
			if ((*it) == in)
			{
				m_frList.erase(it);
				return;
			}
			++it;
		}

	}

	void Update(float deltaT)
	{
		//flex rigid system update lop
		for (std::vector<std::shared_ptr<FlexRigidNode>>::iterator it = m_frList.begin(); it != m_frList.end();)
		{
			// always set positions to flush any other updates (i.e. mouse picking)
			flexSetParticles((*it)->flexBase->m_flex, &(*it)->flexRigid->m_positions[0].x, (*it)->flexRigid->m_positions.size(), eFlexMemoryHost);
			flexSetVelocities((*it)->flexBase->m_flex, &(*it)->flexRigid->m_velocities[0].x, (*it)->flexRigid->m_velocities.size(), eFlexMemoryHost);
			flexSetPhases((*it)->flexBase->m_flex, &(*it)->flexRigid->m_phases[0], (*it)->flexRigid->m_phases.size(), eFlexMemoryHost);

			flexSetParams((*it)->flexBase->m_flex, &(*it)->flexRigid->m_params);
			flexUpdateSolver((*it)->flexBase->m_flex, deltaT, (*it)->flexBase->m_numSubsteps, NULL);

			//now get
			// need up to date positions host side for interaction / debug rendering
			flexGetParticles((*it)->flexBase->m_flex, &(*it)->flexRigid->m_positions[0].x, (*it)->flexRigid->m_positions.size(), eFlexMemoryHost);
			flexGetVelocities((*it)->flexBase->m_flex, &(*it)->flexRigid->m_velocities[0].x, (*it)->flexRigid->m_velocities.size(), eFlexMemoryHost);
			flexGetNormals((*it)->flexBase->m_flex, &(*it)->flexRigid->m_normals[0].x, (*it)->flexRigid->m_normals.size(), eFlexMemoryHost);
			if ((*it)->flexRigid->m_rigidOffsets.size())
				flexGetRigidTransforms((*it)->flexBase->m_flex, (float*)&(*it)->flexRigid->m_rigidRotations[0], (float*)&(*it)->flexRigid->m_rigidTranslations[0], eFlexMemoryHost);

			//update the render mesh vertices with the current deformation
			flexutil::FindSoftBodyOffsets((*it)->flexRigid.get(), (*it)->render.get());

			it++;
		}
	}

	void Init(std::shared_ptr<FlexRigidNode> in)
	{
		flexSetParams(in->flexBase->m_flex, &in->flexRigid->m_params);

		flexSetParticles(in->flexBase->m_flex, (float*)&in->flexRigid->m_positions[0], in->flexRigid->m_positions.size(), eFlexMemoryHost);
		flexSetVelocities(in->flexBase->m_flex, (float*)&in->flexRigid->m_velocities[0], in->flexRigid->m_velocities.size(), eFlexMemoryHost);
		flexSetNormals(in->flexBase->m_flex, (float*)&in->flexRigid->m_normals[0], in->flexRigid->m_normals.size(), eFlexMemoryHost);

		in->flexRigid->m_activeIndices.resize(in->flexRigid->m_positions.size());
		for (size_t i = 0; i < in->flexRigid->m_activeIndices.size(); ++i)
			in->flexRigid->m_activeIndices[i] = i;

		flexSetActive(in->flexBase->m_flex, &in->flexRigid->m_activeIndices[0], in->flexRigid->m_activeIndices.size(), eFlexMemoryHost);

		flexSetPhases(in->flexBase->m_flex, &in->flexRigid->m_phases[0], in->flexRigid->m_phases.size(), eFlexMemoryHost);

		// save rest positions
		in->flexRigid->m_restPositions = in->flexRigid->m_positions;

		flexSetRestParticles(in->flexBase->m_flex, (float*)&in->flexRigid->m_restPositions[0], in->flexRigid->m_restPositions.size(), eFlexMemoryHost);

		// rigids
		if (in->flexRigid->m_rigidOffsets.size())
		{
			assert(in->flexRigid->m_rigidOffsets.size() > 1);

			const int numRigids = in->flexRigid->m_rigidOffsets.size() - 1;

			// calculate local rest space positions
			in->flexRigid->m_rigidLocalPositions.resize(in->flexRigid->m_rigidOffsets.back());
			flexutil::CalculateRigidOffsets(&in->flexRigid->m_positions[0], &in->flexRigid->m_rigidOffsets[0], &in->flexRigid->m_rigidIndices[0], numRigids, &in->flexRigid->m_rigidLocalPositions[0]);

			in->flexRigid->m_rigidRotations.resize(in->flexRigid->m_rigidOffsets.size() - 1, Quat());
			in->flexRigid->m_rigidTranslations.resize(in->flexRigid->m_rigidOffsets.size() - 1, Vec3());

			flexSetRigids(in->flexBase->m_flex, &in->flexRigid->m_rigidOffsets[0], &in->flexRigid->m_rigidIndices[0], (float*)&in->flexRigid->m_rigidLocalPositions[0], in->flexRigid->m_rigidLocalNormals.size() ? (float*)&in->flexRigid->m_rigidLocalNormals[0] : NULL, &in->flexRigid->m_rigidCoefficients[0], (float*)&in->flexRigid->m_rigidRotations[0], (float*)&in->flexRigid->m_rigidTranslations[0], numRigids, eFlexMemoryHost);
		}
	}

};

class FlexClothSystem : public ISystem
{
private:
	std::vector <std::shared_ptr<FlexClothNode>> m_fcList;

public:

	void AddNode(std::shared_ptr<FlexClothNode> in)
	{
		//add a node to the system
		m_fcList.push_back(in);
	}

	void RemoveNode(std::shared_ptr<FlexClothNode> in)
	{
		//remove a node from the system
		for (std::vector<std::shared_ptr<FlexClothNode>>::iterator it = m_fcList.begin(); it != m_fcList.end();)
		{
			if ((*it) == in)
			{
				m_fcList.erase(it);
				return;
			}
			++it;
		}

	}

	void Update(float deltaT)
	{
		//flex rigid system update lop
		for (std::vector<std::shared_ptr<FlexClothNode>>::iterator it = m_fcList.begin(); it != m_fcList.end();)
		{
			// always set positions to flush any other updates (i.e. mouse picking)
			flexSetParticles((*it)->flexBase->m_flex, &(*it)->flexCloth->m_positions[0].x, (*it)->flexCloth->m_positions.size(), eFlexMemoryHost);
			flexSetVelocities((*it)->flexBase->m_flex, &(*it)->flexCloth->m_velocities[0].x, (*it)->flexCloth->m_velocities.size(), eFlexMemoryHost);
			flexSetPhases((*it)->flexBase->m_flex, &(*it)->flexCloth->m_phases[0], (*it)->flexCloth->m_phases.size(), eFlexMemoryHost);

			flexSetParams((*it)->flexBase->m_flex, &(*it)->flexCloth->m_params);
			flexUpdateSolver((*it)->flexBase->m_flex, deltaT, (*it)->flexBase->m_numSubsteps, NULL);

			//now get
			// need up to date positions host side for interaction / debug rendering
			flexGetParticles((*it)->flexBase->m_flex, &(*it)->flexCloth->m_positions[0].x, (*it)->flexCloth->m_positions.size(), eFlexMemoryHost);
			flexGetVelocities((*it)->flexBase->m_flex, &(*it)->flexCloth->m_velocities[0].x, (*it)->flexCloth->m_velocities.size(), eFlexMemoryHost);
			flexGetNormals((*it)->flexBase->m_flex, &(*it)->flexCloth->m_normals[0].x, (*it)->flexCloth->m_normals.size(), eFlexMemoryHost);
			if ((*it)->flexCloth->m_triangles.size())
				flexGetDynamicTriangles((*it)->flexBase->m_flex, &(*it)->flexCloth->m_triangles[0], &(*it)->flexCloth->m_triangleNormals[0].x, (*it)->flexCloth->m_triangles.size() / 3, eFlexMemoryHost);

			it++;
		}
	}

	void Init(std::shared_ptr<FlexClothNode> in)
	{
		flexSetParams(in->flexBase->m_flex, &in->flexCloth->m_params);

		// initialize normals (just for rendering before simulation starts)
		in->flexCloth->m_normals.resize(in->flexCloth->m_positions.size());
		int numTris = in->flexCloth->m_triangles.size() / 3;
		for (int i = 0; i < numTris; ++i)
		{
			Vec3 v0 = Vec3(in->flexCloth->m_positions[in->flexCloth->m_triangles[i * 3 + 0]]);
			Vec3 v1 = Vec3(in->flexCloth->m_positions[in->flexCloth->m_triangles[i * 3 + 1]]);
			Vec3 v2 = Vec3(in->flexCloth->m_positions[in->flexCloth->m_triangles[i * 3 + 2]]);

			Vec3 n = Cross(v1 - v0, v2 - v0);

			in->flexCloth->m_normals[in->flexCloth->m_triangles[i * 3 + 0]] += Vec4(n, 0.0f);
			in->flexCloth->m_normals[in->flexCloth->m_triangles[i * 3 + 1]] += Vec4(n, 0.0f);
			in->flexCloth->m_normals[in->flexCloth->m_triangles[i * 3 + 2]] += Vec4(n, 0.0f);
		}

		for (int i = 0; i < in->flexCloth->m_normals.size(); ++i)
			in->flexCloth->m_normals[i] = Vec4(SafeNormalize(Vec3(in->flexCloth->m_normals[i]), Vec3(0.0f, 1.0f, 0.0f)), 0.0f);

		flexSetParticles(in->flexBase->m_flex, (float*)&in->flexCloth->m_positions[0], in->flexCloth->m_positions.size(), eFlexMemoryHost);
		flexSetVelocities(in->flexBase->m_flex, (float*)&in->flexCloth->m_velocities[0], in->flexCloth->m_velocities.size(), eFlexMemoryHost);
		flexSetNormals(in->flexBase->m_flex, (float*)&in->flexCloth->m_normals[0], in->flexCloth->m_normals.size(), eFlexMemoryHost);

		in->flexCloth->m_activeIndices.resize(in->flexCloth->m_positions.size());
		for (size_t i = 0; i < in->flexCloth->m_activeIndices.size(); ++i)
			in->flexCloth->m_activeIndices[i] = i;

		flexSetActive(in->flexBase->m_flex, &in->flexCloth->m_activeIndices[0], in->flexCloth->m_activeIndices.size(), eFlexMemoryHost);

		// springs
		if (in->flexCloth->m_springIndices.size())
		{
			assert((in->flexCloth->m_springIndices.size() & 1) == 0);
			assert((in->flexCloth->m_springIndices.size() / 2) == in->flexCloth->m_springLengths.size());
			flexSetSprings(in->flexBase->m_flex, &in->flexCloth->m_springIndices[0], &in->flexCloth->m_springLengths[0], &in->flexCloth->m_springStiffness[0], in->flexCloth->m_springLengths.size(), eFlexMemoryHost);
		}

		if (in->flexCloth->m_triangles.size())
		{
			flexSetDynamicTriangles(in->flexBase->m_flex, &in->flexCloth->m_triangles[0], &in->flexCloth->m_triangleNormals[0].x, in->flexCloth->m_triangles.size() / 3, eFlexMemoryHost);
		}

		flexSetPhases(in->flexBase->m_flex, &in->flexCloth->m_phases[0], in->flexCloth->m_phases.size(), eFlexMemoryHost);

		// save rest positions
		in->flexCloth->m_restPositions = in->flexCloth->m_positions;

		flexSetRestParticles(in->flexBase->m_flex, (float*)&in->flexCloth->m_restPositions[0], in->flexCloth->m_restPositions.size(), eFlexMemoryHost);
	}

};

#endif