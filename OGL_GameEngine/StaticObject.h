#pragma once
#ifndef _STATICOBJECT_H_
#define _STATICOBJECT_H_

#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class StaticObject : public GameObject
{
public:
	StaticObject(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName), m_gameEE(gameEE)
	{
		m_transformComp = new TransformComponent(initPos, initRot, true);
		m_transformNode = new TransformNode(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);
	}

	~StaticObject()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveTransformNode(m_transformNode);
		delete m_transformNode;
		delete m_transformComp;
	}

	void Update(float deltaT)
	{
	}

private:

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;

};

#endif