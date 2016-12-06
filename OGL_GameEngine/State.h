#pragma once
#ifndef _STATE_H_
#define _STATE_H_

#include <SDL.h>

class StateManager;
class SDLInputHandler;

class State
{
public:
	State(StateManager* _manager, SDL_Window* _window, SDLInputHandler* _GEinput);

	virtual bool Init() = 0;
	virtual void Update(float deltaT) = 0;
	virtual void Shutdown() = 0;

	SDLInputHandler* GetInput();
	SDL_Window* GetWindow();
	StateManager* GetStateManager();

private:
	SDLInputHandler* m_GEinput;
	StateManager* m_stateManager;
	SDL_Window* m_window;
};
#endif