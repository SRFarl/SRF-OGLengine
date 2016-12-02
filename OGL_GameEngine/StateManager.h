#pragma once
#ifndef _STATEMANAGER_H_
#define _STATEMANAGER_H_

#include "Headers.h"

class State;

class StateManager
{
public:
	~StateManager();

	void AddState(State* _state);
	void ChangeState(State* _state);
	void RemoveLastState();

	bool CheckIfEmpty();

	bool Init();
	void Update(float deltaT);

	
private:
	std::vector<State*> m_states;
};
#endif