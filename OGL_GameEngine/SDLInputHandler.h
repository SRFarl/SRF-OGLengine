#pragma once
#ifndef _SDLINPUTHANDLER_H_
#define _SDLINPUTHANDLER_H_

#include "Headers.h"

struct SDLKey
{
	SDL_Keycode keyEnum;
	bool pressed;
};

class SDLInputHandler
{
public:
	SDLInputHandler(SDL_Keycode keyList[], int keyListSize);
	~SDLInputHandler();

	void UpdateSDLInputs();
	bool getKeyPressed(SDL_Keycode key);
	float getMouseXMotion() { return m_xMoused; };
	float getMouseYMotion() { return m_yMoused; };
	bool getQuitPressed() { return m_bquitPressed; };
private:
	SDL_Event m_theEvent;
	std::list<SDLKey> m_liskeyArray;
	bool m_bquitPressed;
	float m_xMoused;
	float m_yMoused;
};

SDLInputHandler::SDLInputHandler(SDL_Keycode keyList[], int keyListSize) : m_bquitPressed(false)
{
	for (int i = 0; i < keyListSize; i++)
	{
		//create a SDLKey
		SDLKey temp;
		temp.keyEnum = keyList[i];
		temp.pressed = false;
		
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

void SDLInputHandler::UpdateSDLInputs()
{
	m_xMoused = 0;
	m_yMoused = 0;

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
					it->pressed = false;

				continue;
			}
		}

		if (m_theEvent.type == SDL_MOUSEMOTION)
		{
			m_xMoused = m_theEvent.motion.xrel;
			m_yMoused = m_theEvent.motion.yrel;

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
	}
}

#endif