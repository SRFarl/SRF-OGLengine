#include "State.h"
#include "StateManager.h"

State::State(StateManager* _manager, SDL_Window* _window, SDLInputHandler* _GEinput) : m_stateManager(_manager), m_window(_window), m_GEinput(_GEinput)
{

}

SDLInputHandler* State::GetInput()
{
	return m_GEinput;
}

SDL_Window* State::GetWindow()
{
	return m_window;
}

StateManager* State::GetStateManager()
{
	return m_stateManager;
}
