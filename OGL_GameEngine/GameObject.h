#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_
#include "Headers.h"
#include "OBJLoader_SRF.h"

class GameObject
{
private:
	const std::string m_gameModelName;

protected:
	glm::mat4 m_modelMatrix;

	glm::vec3 m_position;

	glm::quat m_quat;
	glm::vec3 m_rotAngles;
	glm::vec3 m_quatRight;
	glm::vec3 m_quatUp;
	glm::vec3 m_quatForward;
	
	bool m_isStatic;

	void UpdateObjectRotation()  //hello
	{
		//QUATERNIONS
		glm::quat quatX = glm::angleAxis(m_rotAngles.x, m_quatRight);

		m_quatUp = quatX * m_quatUp;
		m_quatForward = quatX * m_quatForward;

		glm::quat quatY = glm::angleAxis(m_rotAngles.y, m_quatUp);

		m_quatRight = quatY * m_quatRight;
		m_quatForward = quatY * m_quatForward;

		glm::quat quatZ = glm::angleAxis(m_rotAngles.z, m_quatForward);

		m_quatRight = quatZ * m_quatRight;
		m_quatUp = quatZ * m_quatUp;

		//periodic re-ortho-normalization is needed to correct our axes

		m_quatUp = glm::cross(m_quatRight, m_quatForward);
		m_quatRight = glm::cross(m_quatForward, m_quatUp);

		//normalise

		m_quatRight = glm::normalize(m_quatRight);
		m_quatUp = glm::normalize(m_quatUp);
		m_quatForward = glm::normalize(m_quatForward);

		m_quat = quatZ * quatY * quatX * m_quat;
	}

public:
	GameObject(const std::string gameModelName, glm::vec3 initPos, glm::vec3 initRot, bool _isStatic) :
		m_gameModelName(gameModelName),
		m_position(initPos),
		m_rotAngles(glm::radians(initRot.x), glm::radians(initRot.y), glm::radians(initRot.z)),
		m_isStatic(_isStatic),
		m_quatRight(1.0f, 0.f, 0.f),
		m_quatUp(0.f, 1.0f, 0.f),
		m_quatForward(0.f, 0.f, 1.0f)
	{
		UpdateObjectRotation();
		m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(m_quat);
	}

	virtual void Update(float deltaT) = 0;

	const std::string getName() { return m_gameModelName; }
};

#endif