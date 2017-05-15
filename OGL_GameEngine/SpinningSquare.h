#pragma once
#ifndef _SPINNINGSQAURE_H_
#define _SPINNINGSQAURE_H_

#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, std::shared_ptr<EntityEngine> gameEE, glm::vec3 initPos, glm::vec3 initRot, float _gravity) : GameObject(gameModelName), m_gameEE(gameEE), gravity(_gravity)
	{
		//create all the components and nodes, then pass them to the entity engine
		m_transformComp = std::make_shared<TransformComponent>(initPos, initRot, !_gravity);
		m_transformNode = std::make_shared<TransformNode>(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = std::make_shared<RenderComponent>(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = std::make_shared<RenderNode>(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = std::make_shared<MovableComponent>();
		m_movableNode = std::make_shared<MovableNode>(m_mComp, m_transformComp);
		gameEE->AddMovableNode(m_movableNode);

		//get the aabb data from the model
		glm::vec3 aabbmin;
		glm::vec3 aabbmax;
		glm::vec3 aabbmid;
		modelAsset->BuildAABB(aabbmin, aabbmax, aabbmid);

		m_iDComp = std::make_shared<IDComponent>(gameModelName);
		m_aabbComp = std::make_shared<AABBCollisionComponent>(aabbmin, aabbmax, aabbmid, 0.7f);
		m_aabbNode = std::make_shared<AABBCollisionNode>(m_aabbComp, m_transformComp, m_iDComp, m_mComp);
		gameEE->AddAABBCollisionNode(m_aabbNode);
	}

	~SpinningSquare()
	{
		m_gameEE->RemoveAABBCollisionNode(m_aabbNode);

		m_gameEE->RemoveRenderNode(m_renderNode);

		m_gameEE->RemoveMovableNode(m_movableNode);

		m_gameEE->RemoveTransformNode(m_transformNode);
	}

	void Update(float deltaT)
	{
		if (gravity)
			//apply a gravity force
			m_movableNode->movable->m_acceleration += glm::vec3(0, -9.8f, 0);
	}

private:
	bool gravity;

private:
	//game objects hold the components and nodes
	std::shared_ptr<AABBCollisionComponent> m_aabbComp;
	std::shared_ptr<IDComponent> m_iDComp;
	std::shared_ptr<AABBCollisionNode> m_aabbNode;

	std::shared_ptr<MovableComponent> m_mComp;
	std::shared_ptr<MovableNode> m_movableNode;

	std::shared_ptr<RenderComponent> m_rComp;
	std::shared_ptr<RenderNode> m_renderNode;

	std::shared_ptr<TransformComponent> m_transformComp;
	std::shared_ptr<TransformNode> m_transformNode;

	std::shared_ptr<EntityEngine> m_gameEE;

};

#endif