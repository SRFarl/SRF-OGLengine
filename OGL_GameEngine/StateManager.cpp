#include "StateManager.h"
#include "State.h"

StateManager::~StateManager()
{
	for (int i = 0; i < m_states.size(); i++)
	{
		State *temp = m_states[i];
		m_states.erase(m_states.begin() + i);
		delete temp; //delete all of the game states at the end for clear up
	}
}

void StateManager::AddState(State* _state)
{
	_state->Init();
	m_states.push_back(_state);
}

void StateManager::ChangeState(State* _state)
{
	for (int i = 0; i < m_states.size(); i++)
	{
		State *temp = m_states[i];
		m_states.erase(m_states.begin() + i);
		delete temp;
	} //get rid of all other states and then add the new one passes

	m_states.clear();
	AddState(_state);
}

void StateManager::RemoveLastState()
{
	delete m_states.back();
	m_states.pop_back();
}

bool StateManager::CheckIfEmpty()
{
	if (m_states.size() == 0)
	{
		return true;
	}

	return false;
}

bool StateManager::Init()
{
	return m_states.back()->Init(); //calls the init in a game state
}

void StateManager::Update(float deltaT)
{
	m_states.back()->Update(deltaT); //calls the update in a game state

	if (m_states.back()->m_killState)
	{
		RemoveLastState();
	}
}


