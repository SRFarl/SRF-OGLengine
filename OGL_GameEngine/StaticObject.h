#pragma once
#ifndef _STATICOBJECT_H_
#define _STATICOBJECT_H_

#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class StaticObject : public GameObject
{
public:
	StaticObject(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, std::shared_ptr<EntityEngine> gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName), m_gameEE(gameEE)
	{
		//create all the components and nodes, then pass them to the entity engine
		m_transformComp = std::make_shared<TransformComponent>(initPos, initRot, true);
		m_transformNode = std::make_shared<TransformNode>(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = std::make_shared<RenderComponent>(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = std::make_shared<RenderNode>(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);
	}

	~StaticObject()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);

		m_gameEE->RemoveTransformNode(m_transformNode);
	}

	void Update(float deltaT)
	{
	}

private:

	//game objects hold the components and nodes
	std::shared_ptr<RenderComponent> m_rComp;
	std::shared_ptr<RenderNode> m_renderNode;

	std::shared_ptr<TransformComponent> m_transformComp;
	std::shared_ptr<TransformNode> m_transformNode;

	std::shared_ptr<EntityEngine> m_gameEE;

};

#endif