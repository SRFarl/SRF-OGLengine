#ifndef _ENTITY_ENGINE_H_
#define _ENTITY_ENGINE_H_
#include "EntitiySystem.h"

//the engine updates the systems

class EntityEngine
{
private:
	std::shared_ptr<RenderSystem> m_renderSystem;
	std::shared_ptr<MovableSystem> m_movableSystem;
	std::shared_ptr<TransformSystem> m_transformSystem;
	std::shared_ptr<CollisionSystem> m_collisionSystem;
	std::shared_ptr<FlexRigidSystem> m_flexRigidSystem;
	std::shared_ptr<FlexClothSystem> m_flexClothSystem;

public:
	EntityEngine()
	{
		m_movableSystem = std::make_shared<MovableSystem>();
		m_transformSystem = std::make_shared<TransformSystem>();
		m_renderSystem = std::make_shared<RenderSystem>();
		m_collisionSystem = std::make_shared<CollisionSystem>();
		m_flexRigidSystem = std::make_shared<FlexRigidSystem>();
		m_flexClothSystem = std::make_shared<FlexClothSystem>();
	}

	~EntityEngine()
	{
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
	void AddRenderNode(std::shared_ptr<RenderNode> in)
	{
		m_renderSystem->AddNode(in);
	}

	void RemoveRenderNode(std::shared_ptr<RenderNode> in)
	{
		m_renderSystem->RemoveNode(in);
	}

	void AddRenderDirectionalLight(std::shared_ptr<DirectionalLight> in)
	{
		m_renderSystem->AddDirectionalLight(in);
	}

	void RemoveRenderDirectionalLight(std::shared_ptr<DirectionalLight> in)
	{
		m_renderSystem->RemoveDirectionalLight(in);
	}

	void AddRenderPointLight(std::shared_ptr<PointLight> in)
	{
		m_renderSystem->AddPointLight(in);
	}

	void RemoveRenderPointLight(std::shared_ptr<PointLight> in)
	{
		m_renderSystem->RemovePointLight(in);
	}

	void AddCamera(std::shared_ptr<Camera> in)
	{
		m_renderSystem->AddCamera(in);
	}

	void RemoveCamera(std::shared_ptr<Camera> in)
	{
		m_renderSystem->RemoveCamera(in);
	}

	void AddSkyBox(std::shared_ptr<SkyBox> in)
	{
		m_renderSystem->AddSkyBox(in);
	}

	void RemoveSkyBox()
	{
		m_renderSystem->RemoveSkyBox();
	}

	//movable
	void AddMovableNode(std::shared_ptr<MovableNode> in)
	{
		m_movableSystem->AddNode(in);
	}

	void RemoveMovableNode(std::shared_ptr<MovableNode> in)
	{
		m_movableSystem->RemoveNode(in);
	}

	//transform
	void AddTransformNode(std::shared_ptr<TransformNode> in)
	{
		m_transformSystem->AddNode(in);
		m_transformSystem->UpdateForOne(in);
	}

	void RemoveTransformNode(std::shared_ptr<TransformNode> in)
	{
		m_transformSystem->RemoveNode(in);
	}

	//collision
	void AddSphereCollisionNode(std::shared_ptr<SphereCollisionNode> in)
	{
		m_collisionSystem->AddSphereCollisionNode(in);
	}

	void RemoveSphereCollisionNode(std::shared_ptr<SphereCollisionNode> in)
	{
		m_collisionSystem->RemoveSphereCollisionNode(in);
	}

	void AddAABBCollisionNode(std::shared_ptr<AABBCollisionNode> in)
	{
		m_collisionSystem->AddAABBCollisionNode(in);
	}

	void RemoveAABBCollisionNode(std::shared_ptr<AABBCollisionNode> in)
	{
		m_collisionSystem->RemoveAABBCollisionNode(in);
	}

	void AddCollisionInfoManager(std::shared_ptr<CollisionInfoManager> in)
	{
		m_collisionSystem->AddCollisionInfoManager(in);
	}

	//flex rigid
	void AddFlexRigidNode(std::shared_ptr<FlexRigidNode> in)
	{
		m_flexRigidSystem->AddNode(in);
		m_flexRigidSystem->Init(in);
	}

	void RemoveFlexRigidNode(std::shared_ptr<FlexRigidNode> in)
	{
		m_flexRigidSystem->RemoveNode(in);
	}

	//flex cloth
	void AddFlexClothNode(std::shared_ptr<FlexClothNode> in)
	{
		m_flexClothSystem->AddNode(in);
		m_flexClothSystem->Init(in);
	}

	void RemoveFlexClothNode(std::shared_ptr<FlexClothNode> in)
	{
		m_flexClothSystem->RemoveNode(in);
	}
};

#endif