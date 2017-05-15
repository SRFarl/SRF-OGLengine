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
#include "FlexClothObject.h"
#include "FlexRigidObject.h"
#include "GUIHandler.h"
#include "SkyBox.h"

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
	std::shared_ptr<Shader> m_sceneShader;

private:
	//managers
	std::shared_ptr<AssetBox> m_assetBox;
	std::shared_ptr<EntityEngine> m_entityEngine;
	std::shared_ptr<GUIHandler> m_guiHandler;
	std::shared_ptr<SkyBox> m_skybox;

private:
	//GameObjects
	std::shared_ptr<FPSCamera> m_fpsCam;
	std::shared_ptr<PointLight> m_mainLight;
	std::shared_ptr<DirectionalLight> m_mainDLight;

	std::vector<std::shared_ptr<GameObject>> m_gOList;
	std::shared_ptr<StaticObject> m_woodFloor;
	std::vector<std::shared_ptr<GameSphere>> m_gameSphereList;

	std::shared_ptr<SpinningSquare> m_spinnningSquare1;
	std::shared_ptr<SpinningSquare> m_spinnningSquare2;

	std::shared_ptr<FlexRigidObject> m_flexrigidobject;
	std::shared_ptr<FlexClothObject> m_flexclothobject;
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
	m_assetBox = std::make_shared<AssetBox>();
	m_entityEngine = std::make_shared<EntityEngine>();
	m_guiHandler = std::make_shared<GUIHandler>("Shaders\\gui_shader.vs", "Shaders\\gui_shader.frag", "Shaders\\font_shader.vs", "Shaders\\font_shader.frag", 48);

	//init gui
	m_guiHandler->LoadSprite("testspriteXD", "testspriteXD.png", "GUI/testspriteXD", glm::vec2(100, 100), glm::radians(0.0f), glm::vec2(400, 400));

	//initalise lights
	m_mainLight = std::make_shared<PointLight>();
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	//pass light to the entity engine
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = std::make_shared<DirectionalLight>();
	m_mainDLight->direction = glm::vec3(-1.0f, -5.0f, 1.0f);
	m_mainDLight->diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	//pass light to the entity engine
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	//initalise camera
	m_fpsCam = std::make_shared<FPSCamera>(glm::vec3(11, 2, -2), glm::vec3(170, 0, 0),8.0f);
	m_entityEngine->AddCamera(m_fpsCam);

	//load skybox
	//m_skybox = new SkyBox("Models/skybox/greenhaze_rt.jpg", "Models/skybox/greenhaze_lf.jpg", "Models/skybox/greenhaze_up.jpg",
	//	"Models/skybox/greenhaze_dn.jpg", "Models/skybox/greenhaze_bk.jpg", "Models/skybox/greenhaze_ft.jpg",
	//	"Shaders\\skybox_shader.vs", "Shaders\\skybox_shader.frag");
	m_skybox = std::make_shared<SkyBox>("Models/skybox2/lakes_rt.jpg", "Models/skybox2/lakes_lf.jpg", "Models/skybox2/lakes_up.jpg",
		"Models/skybox2/lakes_dn.jpg", "Models/skybox2/lakes_bk.jpg", "Models/skybox2/lakes_ft.jpg",
		"Shaders\\skybox_shader.vs", "Shaders\\skybox_shader.frag");

	m_entityEngine->AddSkyBox(m_skybox);

	//load assets
	//m_assetBox->LoadAsset("cube", "Models/brickbox/brickbox.obj", false);
	m_assetBox->LoadAsset("sphere", "Models/Sphere/Sphere.obj", false);
	m_assetBox->LoadAsset("flexsphere", "Models/Sphere/Sphere.obj", false);
	m_assetBox->LoadAsset("woodfloor", "Models/woodfloor/woodfloor.obj", false);
	m_assetBox->LoadAsset("cloth", "Models/cloth/cloth.obj", true);

	////create some game spheres
	//for (int i = 0; i < 10; i+=2)
	//{
	//	m_gameSphereList.push_back(new GameSphere("sphere" + i, m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(i, 8, 0), glm::vec3(0, 0, 0), true));
	//	m_gOList.push_back(m_gameSphereList[i]);
	//	m_gameSphereList.push_back(new GameSphere("sphere" + (i+1), m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(i, 2, 0), glm::vec3(0, 0, 0), false));
	//	m_gOList.push_back(m_gameSphereList[i+1]);
	//}
	//
	////create some cubes
	//m_spinnningSquare1 = new SpinningSquare("spin_square1", m_assetBox->GetAsset("cube"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(2.2f, 8, -3), glm::vec3(0, 0, 0), true);
	//m_gOList.push_back(m_spinnningSquare1);
	//m_spinnningSquare2 = new SpinningSquare("spin_square2", m_assetBox->GetAsset("cube"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(2, 2, -3), glm::vec3(0, 0, 0), false);
	//m_gOList.push_back(m_spinnningSquare2);
	//
	////create a floor
	m_woodFloor = std::make_shared<StaticObject>("floor", m_assetBox->GetAsset("woodfloor"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_woodFloor);

	//create flex rigid
	//m_flexrigidobject = std::make_shared<FlexRigidObject>("flexsphere", m_assetBox->GetAsset("flexsphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(3, 3, 2), glm::vec3(0, 0, 0));
	//m_gOList.push_back(m_flexrigidobject);

	//create flex cloth
	m_flexclothobject = std::make_shared<FlexClothObject>("cloth", m_assetBox->GetAsset("cloth"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(3, 3, 2), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_flexclothobject);

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

	////fire a sphere if the user presses f
	//if (GetInput()->getKeyLifted(SDLK_f))
	//	FireSphereFromCamera();

	//update the game objects
	for (std::vector<std::shared_ptr<GameObject>>::iterator it = m_gOList.begin(); it != m_gOList.end();)
	{
		(*it)->Update(deltaTime);
		it++;
	}

	////check if the mouse is hovering over any spheres
	//for (std::vector<GameSphere*>::iterator it = m_gameSphereList.begin(); it != m_gameSphereList.end();)
	//{
	//	(*it)->UpdateSelected(m_fpsCam->GetPos(), m_fpsCam->CreateMouseRayFromCamera(GetInput()->getMouseXPos(), GetInput()->getMouseYPos()));
	//	it++;
	//}
	//
	////removes spheres below a y value
	//RemoveSpheresThatHaveFallen();

	//update the entity engine, this updates all the nodes
	m_entityEngine->Update(deltaTime);

	m_guiHandler->DrawGUI();

	m_guiHandler->RenderText("This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

	SDL_GL_SwapWindow(GetWindow());
}

void GameState::Shutdown()
{
}

bool GameState::BInitShaders()
{
	//create the shader and check that it's working
	m_sceneShader = std::make_shared<Shader>("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

void GameState::FireSphereFromCamera()
{
	//create sphere
	m_gameSphereList.push_back(std::make_shared<GameSphere>("sphereFromCam", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, m_fpsCam->GetPos(), glm::vec3(0, 0, 0), true));
	//give to GameObjectList
	m_gOList.push_back(m_gameSphereList.back());
	//give it an impulse
	m_gameSphereList.back()->GiveAcceleration(m_fpsCam->CreateMouseRayFromCamera(GetInput()->getMouseXPos(), GetInput()->getMouseYPos()) * 1000.0f);
}

void GameState::RemoveSpheresThatHaveFallen()
{
	//loop over every sphere checking that it's above a certain point.
	for (std::vector<std::shared_ptr<GameSphere>>::iterator it = m_gameSphereList.begin(); it != m_gameSphereList.end();)
	{
		if ((*it)->IsBelowYPosition(-20.0f))
		{
			//delete the sphere
			for (std::vector<std::shared_ptr<GameObject>>::iterator it2 = m_gOList.begin(); it2 != m_gOList.end();)
			{
				if (*it2 == *it)
				{
					m_gOList.erase(it2);
					break;
				}
				it2++;
			}

			it = m_gameSphereList.erase(it);
		}
		else
		{
			it++;
		}
	}

}

#endif