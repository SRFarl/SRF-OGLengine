#pragma once
#ifndef _SDLFRAMERATECONTROLLER_H_
#define _SDLFRAMERATECONTROLLER_H_

#include "Headers.h"

class SDLFrameRateController
{
public:
	SDLFrameRateController(unsigned int frameRate);
	void StartFrame() /*MUST BE CALLED AT THE START OF THE GAMELOOP */ { m_ulstartF = SDL_GetTicks(); };
	void DelayForFrameRate();
	void DisplayDebugInfo();
	float getDeltaT() { if (m_bisThereALimit) return m_fchangeThisFrame; else return m_fchangeLastFrame; }
private:
	bool m_bisThereALimit;
	unsigned int m_uiframeRate;	//f/s
	unsigned long m_ulstartF;	//ms
	float m_fchangeThisFrame;	//ms
	float m_fchangeLastFrame;	//ms

private:
	float m_fdebugTimeSpentProcessing;
	float m_fdebugTimeDelayed;
	float m_fdebugFPS;
};

SDLFrameRateController::SDLFrameRateController(unsigned int frameRate) : m_ulstartF(0), m_fdebugTimeSpentProcessing(0), m_fdebugTimeDelayed(0), m_fdebugFPS(0)
{ 
	//initialise frame rate handler
	//if frame rate is set
	if (frameRate != 0)
	{
		m_fchangeThisFrame = (1.0f / frameRate);
		m_uiframeRate = frameRate;
		m_bisThereALimit = true;
	}
	else
	{
		m_bisThereALimit = false;
	}
};

/*MUST BE THE LAST CALL IN THE GAMELOOP*/
void SDLFrameRateController::DelayForFrameRate()
{
	//delays for the rest of the frame based on frames per second
	int timeTakenForFrame = (SDL_GetTicks() - m_ulstartF);
	m_fchangeLastFrame = timeTakenForFrame / 1000.f;
	m_fdebugTimeSpentProcessing = timeTakenForFrame;
	if (m_bisThereALimit)
	{
		//delay based on the limit
		//get debug information
		if (timeTakenForFrame < ((1.0f / m_uiframeRate)*1000.f))
		{
			unsigned int timeToDelay = (unsigned int)(((1.0f / m_uiframeRate)*1000.f) - timeTakenForFrame);
			m_fdebugTimeDelayed = timeToDelay;
			m_fdebugFPS = m_uiframeRate;
			SDL_Delay(timeToDelay);
		}
		else
		{
			m_fdebugTimeDelayed = 0;
			m_fdebugFPS = 1000 / timeTakenForFrame;
		}
	}
	else
	{
		//don't delay
		//get debug information
		m_fdebugTimeDelayed = 0;
		if (timeTakenForFrame > 0)
		{
			m_fdebugFPS = 1000 / timeTakenForFrame;
		}
		else
		{
			m_fdebugFPS = 1001;
		}
	}
}


void SDLFrameRateController::DisplayDebugInfo()
{
	printf("FPS: %3.1f p/s, Time spent processing: %1.0f ms, Time spent delayed: %1.0f ms \n", m_fdebugFPS, m_fdebugTimeSpentProcessing, m_fdebugTimeDelayed);
}

#endif