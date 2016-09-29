#ifndef _ENTITY_ENGINE_H_
#define _ENTITY_ENGINE_H_
#include "EntitiySystem.h"

class EntityEngine
{
private:
	RenderSystem* m_renderSystem;
	MovableSystem* m_movableSystem;

public:
	EntityEngine()
	{
		m_renderSystem = new RenderSystem();
		m_movableSystem = new MovableSystem();
	}

	~EntityEngine()
	{
		delete m_renderSystem;
		delete m_movableSystem;
	}

	void Update(float deltaT)
	{
		m_renderSystem->Update(deltaT);
		m_movableSystem->Update(deltaT);
	}

	//render
	void AddRenderNode(RenderNode* in)
	{
		m_renderSystem->AddNode(in);
	}

	void RemoveRenderNode(RenderNode* in)
	{
		m_renderSystem->RemoveNode(in);
	}

	void AddRenderDirectionalLight(DirectionalLight* in)
	{
		m_renderSystem->AddDirectionalLight(in);
	}

	void RemoveRenderDirectionalLight(DirectionalLight* in)
	{
		m_renderSystem->RemoveDirectionalLight(in);
	}

	void AddRenderPointLight(PointLight* in)
	{
		m_renderSystem->AddPointLight(in);
	}

	void RemoveRenderPointLight(PointLight* in)
	{
		m_renderSystem->RemovePointLight(in);
	}

	void AddCamera(FPSCamera* in)
	{
		m_renderSystem->AddCamera(in);
	}

	void RemoveCamera(FPSCamera* in)
	{
		m_renderSystem->RemoveCamera(in);
	}

	//movable
	void AddMovableNode(MovableNode* in)
	{
		m_movableSystem->AddNode(in);
	}

	void RemoveMovableNode(MovableNode* in)
	{
		m_movableSystem->RemoveNode(in);
	}

};

#endif