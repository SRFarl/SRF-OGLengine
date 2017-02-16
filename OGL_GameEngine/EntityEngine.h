#ifndef _ENTITY_ENGINE_H_
#define _ENTITY_ENGINE_H_
#include "EntitiySystem.h"

//the engine updates the systems

class EntityEngine
{
private:
	RenderSystem* m_renderSystem;
	MovableSystem* m_movableSystem;
	TransformSystem* m_transformSystem;
	CollisionSystem* m_collisionSystem;
	FlexRigidSystem* m_flexRigidSystem;
	FlexClothSystem* m_flexClothSystem;

public:
	EntityEngine()
	{
		m_movableSystem = new MovableSystem();
		m_transformSystem = new TransformSystem();
		m_renderSystem = new RenderSystem();
		m_collisionSystem = new CollisionSystem();
		m_flexRigidSystem = new FlexRigidSystem();
		m_flexClothSystem = new FlexClothSystem();
	}

	~EntityEngine()
	{
		delete m_flexClothSystem;
		delete m_flexRigidSystem;
		delete m_collisionSystem;
		delete m_transformSystem;
		delete m_renderSystem;
		delete m_movableSystem;
	}

	void Update(float deltaT)
	{
		m_flexClothSystem->Update(deltaT);
		m_flexRigidSystem->Update(deltaT);
		m_collisionSystem->Update(deltaT);
		m_movableSystem->Update(deltaT);
		m_transformSystem->Update(deltaT);
		m_renderSystem->Update(deltaT);
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

	//transform
	void AddTransformNode(TransformNode* in)
	{
		m_transformSystem->AddNode(in);
		m_transformSystem->UpdateForOne(in);
	}

	void RemoveTransformNode(TransformNode* in)
	{
		m_transformSystem->RemoveNode(in);
	}

	//collision
	void AddSphereCollisionNode(SphereCollisionNode* in)
	{
		m_collisionSystem->AddSphereCollisionNode(in);
	}

	void RemoveSphereCollisionNode(SphereCollisionNode* in)
	{
		m_collisionSystem->RemoveSphereCollisionNode(in);
	}

	void AddAABBCollisionNode(AABBCollisionNode* in)
	{
		m_collisionSystem->AddAABBCollisionNode(in);
	}

	void RemoveAABBCollisionNode(AABBCollisionNode* in)
	{
		m_collisionSystem->RemoveAABBCollisionNode(in);
	}

	//flex rigid
	void AddFlexRigidNode(FlexRigidNode* in)
	{
		m_flexRigidSystem->AddNode(in);
		m_flexRigidSystem->Init(in);
	}

	void RemoveFlexRigidNode(FlexRigidNode* in)
	{
		m_flexRigidSystem->RemoveNode(in);
	}

	//flex cloth
	void AddFlexClothNode(FlexClothNode* in)
	{
		m_flexClothSystem->AddNode(in);
		m_flexClothSystem->Init(in);
	}

	void RemoveFlexClothNode(FlexClothNode* in)
	{
		m_flexClothSystem->RemoveNode(in);
	}
};

#endif