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
	
	bool m_isStatic;

public:
	GameObject(const std::string gameModelName, glm::vec3 initPos, bool _isStatic) :
		m_gameModelName(gameModelName),
		m_position(initPos),
		m_isStatic(_isStatic)
	{
		m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
	}

	virtual void Update(float deltaT) = 0;

	const std::string getName() { return m_gameModelName; }
};

#endif