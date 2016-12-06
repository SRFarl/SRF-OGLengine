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
#include "PauseState.h"

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
	void FireSphereFromCamera();
	void RemoveSpheresThatHaveFallen();

private: 
	//GL
	Shader *m_sceneShader;

private:
	//managers
	AssetBox *m_assetBox;
	EntityEngine *m_entityEngine;

private:
	//GameObjects
	FPSCamera* m_fpsCam;
	PointLight* m_mainLight;
	DirectionalLight* m_mainDLight;

	std::vector<GameObject*> m_gOList;
	StaticObject* m_woodFloor;
	std::vector<GameSphere*> m_gameSphereList;

	SpinningSquare* m_spinnningSquare1;
	SpinningSquare* m_spinnningSquare2;
};

GameState::GameState(StateManager* _stateManager, SDL_Window* _window, SDLInputHandler* _GEinput) : State(_stateManager, _window, _GEinput)
{

}

bool GameState::Init()
{
	//initialise shaders
	if (!BInitShaders())
	{
		return false;
	}

	//create managers
	m_assetBox = new AssetBox();
	m_entityEngine = new EntityEngine();

	//initalise lights
	m_mainLight = new PointLight;
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	//pass light to the entity engine
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = new DirectionalLight;
	m_mainDLight->direction = glm::vec3(-1.0f, -5.0f, 1.0f);
	m_mainDLight->diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	//pass light to the entity engine
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	//initalise camera
	m_fpsCam = new FPSCamera(glm::vec3(11, 2, -2), glm::vec3(170, 0, 0),8.0f);
	m_entityEngine->AddCamera(m_fpsCam);

	//load assets
	m_assetBox->LoadAsset("cube", "Models/brickbox/brickbox.obj");
	m_assetBox->LoadAsset("sphere", "Models/Sphere/Sphere.obj");
	m_assetBox->LoadAsset("woodfloor", "Models/woodfloor/woodfloor.obj");

	//create some game spheres
	for (int i = 0; i < 10; i+=2)
	{
		m_gameSphereList.push_back(new GameSphere("sphere" + i, m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(i, 8, 0), glm::vec3(0, 0, 0), true));
		m_gOList.push_back(m_gameSphereList[i]);
		m_gameSphereList.push_back(new GameSphere("sphere" + (i+1), m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(i, 2, 0), glm::vec3(0, 0, 0), false));
		m_gOList.push_back(m_gameSphereList[i+1]);
	}

	//create some cubes
	m_spinnningSquare1 = new SpinningSquare("spin_square1", m_assetBox->GetAsset("cube"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(2.2f, 8, -3), glm::vec3(0, 0, 0), true);
	m_gOList.push_back(m_spinnningSquare1);
	m_spinnningSquare2 = new SpinningSquare("spin_square2", m_assetBox->GetAsset("cube"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(2, 2, -3), glm::vec3(0, 0, 0), false);
	m_gOList.push_back(m_spinnningSquare2);

	//create a floor
	m_woodFloor = new StaticObject("floor", m_assetBox->GetAsset("woodfloor"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, -1, 0), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_woodFloor);

	return true;
}

void GameState::Update(float deltaTime)
{
	//check if game is paused
	if (GetInput()->getKeyLifted(SDLK_p))
	{
		GetStateManager()->AddState(new PauseState(GetStateManager(), GetWindow(), GetInput()));
		return;
	}

	//update camera
	m_fpsCam->UpdateCamera(deltaTime, GetInput()->getKeyPressed(SDLK_w),
		GetInput()->getKeyPressed(SDLK_s), GetInput()->getKeyPressed(SDLK_a), GetInput()->getKeyPressed(SDLK_d), GetInput()->getMouseLDown(),
		GetInput()->getMouseXMotion(), GetInput()->getMouseYMotion());

	//fire a sphere if the user presses f
	if (GetInput()->getKeyLifted(SDLK_f))
		FireSphereFromCamera();

	//update the game objects
	for (std::vector<GameObject*>::iterator it = m_gOList.begin(); it != m_gOList.end();)
	{
		(*it)->Update(deltaTime);
		it++;
	}

	//check if the mouse is hovering over any spheres
	for (std::vector<GameSphere*>::iterator it = m_gameSphereList.begin(); it != m_gameSphereList.end();)
	{
		(*it)->UpdateSelected(m_fpsCam->GetPos(), m_fpsCam->CreateMouseRayFromCamera(GetInput()->getMouseXPos(), GetInput()->getMouseYPos()));
		it++;
	}

	//removes spheres below a y value
	RemoveSpheresThatHaveFallen();

	//update the entity engine, this updates all the nodes
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

	for (int i = 0; i < m_gameSphereList.size();)
	{
		GameSphere *temp = m_gameSphereList[i];
		m_gameSphereList.erase(m_gameSphereList.begin() + i);
		delete temp; //delete
	}

	delete m_spinnningSquare1;
	delete m_spinnningSquare2;

	//clean app classes
	delete m_sceneShader;
	delete m_entityEngine;
	delete m_assetBox;
}

bool GameState::BInitShaders()
{
	//create the shader and check that it's working
	m_sceneShader = new Shader("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

void GameState::FireSphereFromCamera()
{
	//create sphere
	m_gameSphereList.push_back(new GameSphere("sphereFromCam", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, m_fpsCam->GetPos(), glm::vec3(0, 0, 0), true));
	//give to GameObjectList
	m_gOList.push_back(m_gameSphereList.back());
	//give it an impulse
	m_gameSphereList.back()->GiveAcceleration(m_fpsCam->CreateMouseRayFromCamera(GetInput()->getMouseXPos(), GetInput()->getMouseYPos()) * 1000.0f);
}

void GameState::RemoveSpheresThatHaveFallen()
{
	//loop over every sphere checking that it's above a certain point.
	for (std::vector<GameSphere*>::iterator it = m_gameSphereList.begin(); it != m_gameSphereList.end();)
	{
		if ((*it)->IsBelowYPosition(-20.0f))
		{
			//delete the sphere
			for (std::vector<GameObject*>::iterator it2 = m_gOList.begin(); it2 != m_gOList.end();)
			{
				if (*it2 == *it)
				{
					m_gOList.erase(it2);
					break;
				}
				it2++;
			}

			delete *it;
			it = m_gameSphereList.erase(it);
		}
		else
		{
			it++;
		}
	}

}

#endif