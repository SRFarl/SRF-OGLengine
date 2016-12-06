#pragma once
#ifndef _GAMESPHERE_H_
#define _GAMESPHERE_H_

#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class GameSphere : public GameObject
{
public:
	GameSphere(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot, bool _gravity) : GameObject(gameModelName), m_gameEE(gameEE), gravity(_gravity)
	{
		//create all the components and nodes, then pass them to the entity engine
		m_selComp = new SelectedComponent();

		m_transformComp = new TransformComponent(initPos, initRot, !_gravity);
		m_transformNode = new TransformNode(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(m_rComp, m_transformComp, m_selComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(m_mComp, m_transformComp);
		gameEE->AddMovableNode(m_movableNode);

		m_scComponent = new SphereCollsionComponent(glm::vec3(0.0f), 1.0f, 0.80f);
		m_scNode = new SphereCollisionNode(m_scComponent, m_transformComp, m_mComp);
		gameEE->AddSphereCollisionNode(m_scNode);
	}

	~GameSphere()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveTransformNode(m_transformNode);
		delete m_transformNode;
		delete m_transformComp;

		m_gameEE->RemoveMovableNode(m_movableNode);
		delete m_mComp;
		delete m_movableNode;

		m_gameEE->RemoveSphereCollisionNode(m_scNode);
		delete m_scComponent;
		delete m_scNode;
	}

	void Update(float deltaT)
	{
		if (gravity)
			//apply a gravity force
			m_movableNode->movable->m_acceleration += glm::vec3(0, -9.8f, 0);
	}

	void UpdateSelected(glm::vec3 camarePos, glm::vec3 mouseDir)
	{
		//sets the selected component if the model intersects with the ray
		if (Math::RaySphereIntersect(mouseDir, camarePos, m_transformComp->m_position + m_scComponent->m_centrePointOffset, m_scComponent->m_radius))
			m_selComp->m_selected = true;
		else
			m_selComp->m_selected = false;
	}

	void GiveAcceleration(glm::vec3 _acceleration)
	{
		//apply an acceleration to this object
		m_movableNode->movable->m_acceleration += _acceleration;
	}

	bool IsBelowYPosition(float y)
	{
		//checks if position is below a certain point
		return m_transformComp->m_position.y < y;
	}

private:
	bool gravity;

private:
	//game objects hold the components and nodes
	SelectedComponent* m_selComp;

	SphereCollsionComponent* m_scComponent;
	SphereCollisionNode* m_scNode;

	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;

};

#endif