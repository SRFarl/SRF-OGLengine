#pragma once
#ifndef _PAUSESTATE_H_
#define _PAUSESTATE_H_

#include "State.h"
#include "StateManager.h"
#include "Headers.h"
#include "StaticObject.h"
#include "AssetBox.h"
#include "SDLInputHandler.h"
#include "Shader.h"

class PauseState : public State
{
public:
	PauseState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput);
	~PauseState();
	bool Init();
	void Update(float deltaTime);
	void Shutdown();
	
private:
	bool BInitShaders();

private: //GL
	Shader *m_sceneShader;

private:
	AssetBox *m_assetBox;
	EntityEngine *m_entityEngine;

private:
	//GameObjects
	FPSCamera* m_fpsCam;
	PointLight* m_mainLight;
	DirectionalLight* m_mainDLight;

	std::vector<GameObject*> m_gOList;
	StaticObject* m_pauseBox;
};

PauseState::PauseState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput) : State(_stateManager, _window, _GEinput)
{

}

PauseState::~PauseState()
{
	Shutdown();
}

bool PauseState::Init()
{
	if (!BInitShaders())
	{
		return false;
	}

	m_assetBox = new AssetBox();
	m_entityEngine = new EntityEngine();

	m_mainLight = new PointLight;
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = new DirectionalLight;
	m_mainDLight->direction = glm::vec3(0.0f, 0.0f, -1.0f);
	m_mainDLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	m_fpsCam = new FPSCamera(glm::vec3(0,0,9), glm::vec3(-90,0,-90), 16.0f);
	m_entityEngine->AddCamera(m_fpsCam);

	//load assets
	m_assetBox->LoadAsset("PausePlane", "Models/PausePlane/PausePlane.obj");

	m_pauseBox = new StaticObject("PausePlane", m_assetBox->GetAsset("PausePlane"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, 0, 0), glm::vec3(glm::radians(90.f), 0, 0));
	m_gOList.push_back(m_pauseBox);

	return true;
}

void PauseState::Update(float deltaTime)
{
	//check if game needs to un-paused
	if (GetInput()->getKeyLifted(SDLK_p))
	{
		GetStateManager()->RemoveLastState();
		return;
	}

	m_fpsCam->UpdateCamera(deltaTime, false, false, false, false, false, 0, 0);

	for (std::vector<GameObject*>::iterator it = m_gOList.begin(); it != m_gOList.end();)
	{
		(*it)->Update(deltaTime);
		it++;
	}

	m_entityEngine->Update(deltaTime);

	SDL_GL_SwapWindow(GetWindow());
}

void PauseState::Shutdown()
{
	//clean game
	delete m_mainLight;
	delete m_mainDLight;
	delete m_pauseBox;
	delete m_fpsCam;

	//clean app classes
	delete m_sceneShader;
	delete m_entityEngine;
	delete m_assetBox;
}

bool PauseState::BInitShaders()
{
	m_sceneShader = new Shader("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

#endif