#pragma once
#ifndef _SPINNINGSQAURE_H_
#define _SPINNINGSQAURE_H_

#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot, float _gravity) : GameObject(gameModelName), m_gameEE(gameEE), gravity(_gravity)
	{
		//create all the components and nodes, then pass them to the entity engine
		m_transformComp = new TransformComponent(initPos, initRot, !_gravity);
		m_transformNode = new TransformNode(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(m_mComp, m_transformComp);
		gameEE->AddMovableNode(m_movableNode);

		//get the aabb data from the model
		glm::vec3 aabbmin;
		glm::vec3 aabbmax;
		glm::vec3 aabbmid;
		modelAsset->BuildAABB(aabbmin, aabbmax, aabbmid);

		m_aabbComp = new AABBCollisionComponent(aabbmin, aabbmax, aabbmid, 0.7f);
		m_aabbNode = new AABBCollisionNode(m_aabbComp, m_transformComp, m_mComp);
		gameEE->AddAABBCollisionNode(m_aabbNode);
	}

	~SpinningSquare()
	{
		m_gameEE->RemoveAABBCollisionNode(m_aabbNode);
		delete m_aabbNode;
		delete m_aabbComp;

		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveMovableNode(m_movableNode);
		delete m_movableNode;
		delete m_mComp;

		m_gameEE->RemoveTransformNode(m_transformNode);
		delete m_transformNode;
		delete m_transformComp;
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
	AABBCollisionComponent* m_aabbComp;
	AABBCollisionNode* m_aabbNode;

	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;

};

#endif