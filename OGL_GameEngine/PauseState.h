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

private: 
	//GL
	Shader *m_sceneShader;

private:
	//managers
	std::shared_ptr<AssetBox> m_assetBox;
	std::shared_ptr<EntityEngine> m_entityEngine;

private:
	//GameObjects
	std::shared_ptr<FPSCamera> m_fpsCam;
	std::shared_ptr<PointLight> m_mainLight;
	std::shared_ptr<DirectionalLight> m_mainDLight;

	std::vector<GameObject*> m_gOList;
	StaticObject* m_pauseBox;
};

PauseState::PauseState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput) : State(_stateManager, _window, _GEinput)
{

}

bool PauseState::Init()
{
	//initialise shaders
	if (!BInitShaders())
	{
		return false;
	}

	//create managers
	m_assetBox = std::make_shared<AssetBox>();
	m_entityEngine = std::make_shared<EntityEngine>();

	//initalise lights
	m_mainLight = std::make_shared<PointLight>();
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	//pass light to the entity engine
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = std::make_shared<DirectionalLight>();
	m_mainDLight->direction = glm::vec3(0.0f, 0.0f, -1.0f);
	m_mainDLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	//pass light to the entity engine
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	//initalise camera
	m_fpsCam = std::make_shared<FPSCamera>(glm::vec3(0,0,9), glm::vec3(-90,0,-90), 16.0f);
	m_entityEngine->AddCamera(m_fpsCam);

	//load assets
	m_assetBox->LoadAsset("PausePlane", "Models/PausePlane/PausePlane.obj", false);

	//create the pause box
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

	//update camera
	m_fpsCam->UpdateCamera(deltaTime, false, false, false, false, false, 0, 0);

	//update the game objects
	for (std::vector<GameObject*>::iterator it = m_gOList.begin(); it != m_gOList.end();)
	{
		(*it)->Update(deltaTime);
		it++;
	}

	//update the entity engine, this updates all the nodes
	m_entityEngine->Update(deltaTime);

	SDL_GL_SwapWindow(GetWindow());
}

void PauseState::Shutdown()
{
	//clean game
	delete m_pauseBox;

	//clean app classes
	delete m_sceneShader;
}

bool PauseState::BInitShaders()
{
	//create the shader and check that it's working
	m_sceneShader = new Shader("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

#endif