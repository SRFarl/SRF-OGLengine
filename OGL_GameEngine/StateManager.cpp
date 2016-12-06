#include "StateManager.h"
#include "State.h"

StateManager::~StateManager()
{
	//remove all the states on deleting the statemanager
	for (int i = 0; i < m_states.size(); i++)
	{
		m_states[i]->Shutdown();
		delete m_states[i];
	}

	m_states.clear();
}

void StateManager::AddState(State* _state)
{
	//initialise then add to the state array
	_state->Init();
	m_states.push_back(_state);
}

void StateManager::ChangeState(State* _state)
{
	//remove the other states
	for (int i = 0; i < m_states.size();)
	{
		m_states[i]->Shutdown();
		delete m_states[i];
	}
	m_states.clear();

	AddState(_state);
}

void StateManager::RemoveLastState()
{
	//shutdown and then delete the state at the back of the list
	m_states.back()->Shutdown();
	delete m_states.back();
	m_states.pop_back();
}

bool StateManager::CheckIfEmpty()
{
	//returns a boolean on whether the manager is empty
	if (m_states.size() == 0)
	{
		return true;
	}

	return false;
}

void StateManager::Update(float deltaT)
{
	//calls the update in a game state
	m_states.back()->Update(deltaT);
}


