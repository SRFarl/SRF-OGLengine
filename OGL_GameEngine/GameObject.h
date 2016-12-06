#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_
#include "Headers.h"
#include "OBJLoader_SRF.h"

class GameObject
{
private:
	const std::string m_gameModelName;

public:
	GameObject(const std::string gameModelName) :
		m_gameModelName(gameModelName)
	{}

	~GameObject()
	{}

	virtual void Update(float deltaT) = 0;

	const std::string getName() { return m_gameModelName; }
};

#endif