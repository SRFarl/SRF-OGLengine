#pragma once
#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "State.h"
#include "StateManager.h"
#include "Headers.h"
#include "GameSphere.h"
#include "StaticObject.h"
#include "AssetBox.h"
#include "SDLInputHandler.h"
#include "Shader.h"
#include "SpinningSquare.h"

class GameState : public State
{
public:
	GameState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput);
	~GameState();
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
	StaticObject* m_woodFloor;
	GameSphere* m_gameSphere1;
	GameSphere* m_gameSphere2;
	SpinningSquare* m_spinnningSquare;
};

GameState::GameState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput) : State(_stateManager, _window, _GEinput)
{

}

GameState::~GameState()
{
	Shutdown();
}

bool GameState::Init()
{
	if (!BInitShaders())
	{
		return false;
	}

	m_assetBox = new AssetBox();
	m_entityEngine = new EntityEngine();

	m_mainLight = new PointLight;
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = new DirectionalLight;
	m_mainDLight->direction = glm::vec3(-1.0f, -5.0f, 1.0f);
	m_mainDLight->diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	m_fpsCam = new FPSCamera(16.0f);
	m_entityEngine->AddCamera(m_fpsCam);

	//load assets
	m_assetBox->LoadAsset("cube", "Models/brickbox/brickbox.obj");
	m_assetBox->LoadAsset("sphere", "Models/Sphere/Sphere.obj");
	m_assetBox->LoadAsset("woodfloor", "Models/woodfloor/woodfloor.obj");

	m_gameSphere1 = new GameSphere("sphere1", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0.1f, 8, 0), glm::vec3(0, 0, 0), true);
	m_gOList.push_back(m_gameSphere1);
	m_gameSphere2 = new GameSphere("sphere2", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), false);
	m_gOList.push_back(m_gameSphere2);

	m_spinnningSquare = new SpinningSquare("spin_square", m_assetBox->GetAsset("cube"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(2, 2, 0), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_spinnningSquare);

	m_woodFloor = new StaticObject("floor", m_assetBox->GetAsset("woodfloor"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, -1, 0), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_woodFloor);

	return true;
}

void GameState::Update(float deltaTime)
{
	m_fpsCam->UpdateCamera(deltaTime, GetInput()->getKeyPressed(SDLK_w),
		GetInput()->getKeyPressed(SDLK_s), GetInput()->getKeyPressed(SDLK_a), GetInput()->getKeyPressed(SDLK_d),
		GetInput()->getMouseXMotion(), GetInput()->getMouseYMotion());

	for (std::vector<GameObject*>::iterator it = m_gOList.begin(); it != m_gOList.end();)
	{
		(*it)->Update(deltaTime);
		it++;
	}

	m_entityEngine->Update(deltaTime);

	SDL_GL_SwapWindow(GetWindow());
}

void GameState::Shutdown()
{
	//clean game
	delete m_mainLight;
	delete m_mainDLight;
	delete m_woodFloor;
	delete m_fpsCam;
	delete m_gameSphere1;
	delete m_gameSphere2;

	//clean app classes
	delete m_sceneShader;
	delete m_entityEngine;
	delete m_assetBox;
}

bool GameState::BInitShaders()
{
	m_sceneShader = new Shader("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

#endif