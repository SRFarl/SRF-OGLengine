#pragma once
#ifndef _SDLINPUTHANDLER_H_
#define _SDLINPUTHANDLER_H_

#include "Headers.h"

struct SDLKey
{
	SDL_Keycode keyEnum;
	bool pressed;
	bool lifted;
};

class SDLInputHandler
{
public:
	SDLInputHandler(SDL_Keycode keyList[], int keyListSize);
	~SDLInputHandler();

	void UpdateSDLInputs();
	bool getKeyPressed(SDL_Keycode key);
	bool getKeyLifted(SDL_Keycode key);
	inline float getMouseLDown() { return m_mouseLDown; };
	inline float getMouseRDown() { return m_mouseRDown; };
	inline float getMouseLClicked() { return m_mouseLClicked; };
	inline float getMouseRClicked() { return m_mouseRClicked; };
	inline float getMouseXMotion() { return m_xMoused; };
	inline float getMouseYMotion() { return m_yMoused; };
	inline float getMouseXPos() { return m_xMousePos; }
	inline float getMouseYPos() { return m_yMousePos; }
	bool getQuitPressed() { return m_bquitPressed; };
private:
	SDL_Event m_theEvent;
	std::list<SDLKey> m_liskeyArray;
	bool m_bquitPressed;
	bool m_mouseLDown;
	bool m_mouseRDown;
	bool m_mouseLClicked;
	bool m_mouseRClicked;
	float m_xMoused;
	float m_yMoused;
	float m_xMousePos;
	float m_yMousePos;
};

SDLInputHandler::SDLInputHandler(SDL_Keycode keyList[], int keyListSize) : m_bquitPressed(false),
	m_mouseLDown(false), m_mouseRDown(false),
	m_mouseLClicked(false), m_mouseRClicked(false)
{
	//initialise the SDLKey list
	for (int i = 0; i < keyListSize; i++)
	{
		//create a SDLKey
		SDLKey temp;
		temp.keyEnum = keyList[i];
		temp.pressed = false;
		temp.lifted = false;
		
		m_liskeyArray.push_back(temp);
	}
}

SDLInputHandler::~SDLInputHandler()
{
	//empty the SDLKey array
	m_liskeyArray.clear();
}

bool SDLInputHandler::getKeyPressed(SDL_Keycode key)
{
	//returns a boolean if a key is registered as pressed
	for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
	{
		//is this the key requested
		if (it->keyEnum == key)
		{
			return it->pressed;
		}
	}

	//Safety net - Will only be used if the key argument isn't in the keyArray
	return false;
}

bool SDLInputHandler::getKeyLifted(SDL_Keycode key)
{
	//returns a boolean if a key is lifted, will only last for 1 frame
	for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
	{
		//is this the key requested
		if (it->keyEnum == key)
		{
			return it->lifted;
		}
	}

	//Safety net - Will only be used if the key argument isn't in the keyArray
	return false;
}

void SDLInputHandler::UpdateSDLInputs()
{
	//updates all the inputs and stores the information
	m_xMoused = 0;
	m_yMoused = 0;

	//reset lifted/clicked
	m_mouseLClicked = false;
	m_mouseRClicked = false;

	for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
	{
		it->lifted = false;
	}

	
	while (SDL_PollEvent(&m_theEvent) != 0)
	{
		//poll over all the events
		if (m_theEvent.type == SDL_QUIT)
		{
			//quit was pressed
			m_bquitPressed = true;
			return;
		}

		for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
		{
			//loop over all the keys we need to look out for
			if (m_theEvent.key.keysym.sym == (it)->keyEnum)
			{
				//this key has had an action
				if (m_theEvent.type == SDL_KEYDOWN)
					it->pressed = true;
				else if (m_theEvent.type == SDL_KEYUP)
				{
					it->lifted = true;
					it->pressed = false;
				}

				continue;
			}
		}

		if (m_theEvent.type == SDL_MOUSEMOTION)
		{
			//there has benn mouse motion
			m_xMoused = m_theEvent.motion.xrel;
			m_yMoused = m_theEvent.motion.yrel;

			m_xMousePos = m_theEvent.motion.x;
			m_yMousePos = m_theEvent.motion.y;

			//clamp mouse move
			if (m_xMoused >= 100.f)
				m_xMoused = 100.f;
			if (m_xMoused <= -100.f)
				m_xMoused = -100.f;
			if (m_yMoused >= 100.f)
				m_yMoused = 100.f;
			if (m_yMoused <= -100.f)
				m_yMoused = -100.f;
		}

		if (m_theEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			//mouse button has been pressed
			if (m_theEvent.button.button == SDL_BUTTON_LEFT)
			{
				//left mouse button was pressed
				m_mouseLDown = true;
				m_mouseLClicked = true;
			}
			else if (m_theEvent.button.button == SDL_BUTTON_RIGHT)
			{
				//right mouse button was pressed
				m_mouseRDown = true;
				m_mouseRClicked = true;
			}
		}

		if (m_theEvent.type == SDL_MOUSEBUTTONUP)
		{
			//mouse button was lifted
			if (m_theEvent.button.button == SDL_BUTTON_LEFT)
			{
				//left mouse button was lifted
				m_mouseLDown = false;
			}
			else if (m_theEvent.button.button == SDL_BUTTON_RIGHT)
			{
				//right mouse button was lifted
				m_mouseRDown = false;
			}
		}
	}
}

#endif