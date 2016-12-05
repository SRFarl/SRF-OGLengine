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
	m_liskeyArray.clear();
}

bool SDLInputHandler::getKeyPressed(SDL_Keycode key)
{
	for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
	{
		if (it->keyEnum == key)
		{
			return it->pressed;
		}
	}

	//Safety net - Will only be used if the key arg isn't in the keyArray
	return false;
}

bool SDLInputHandler::getKeyLifted(SDL_Keycode key)
{
	for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
	{
		if (it->keyEnum == key)
		{
			return it->lifted;
		}
	}

	//Safety net - Will only be used if the key arg isn't in the keyArray
	return false;
}

void SDLInputHandler::UpdateSDLInputs()
{
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
		if (m_theEvent.type == SDL_QUIT)
		{
			m_bquitPressed = true;
			return;
		}

		for (std::list<SDLKey>::iterator it = m_liskeyArray.begin(); it != m_liskeyArray.end(); it++)
		{
			if (m_theEvent.key.keysym.sym == (it)->keyEnum)
			{
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
			if (m_theEvent.button.button == SDL_BUTTON_LEFT)
			{
				m_mouseLDown = true;
				m_mouseLClicked = true;
			}
			else if (m_theEvent.button.button == SDL_BUTTON_RIGHT)
			{
				m_mouseRDown = true;
				m_mouseRClicked = true;
			}
		}

		if (m_theEvent.type == SDL_MOUSEBUTTONUP)
		{
			if (m_theEvent.button.button == SDL_BUTTON_LEFT)
			{
				m_mouseLDown = false;
			}
			else if (m_theEvent.button.button == SDL_BUTTON_RIGHT)
			{
				m_mouseRDown = false;
			}
		}
	}
}

#endif