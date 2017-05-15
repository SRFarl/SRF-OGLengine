#ifndef _ENTITY_NODES_H_
#define _ENTITY_NODES_H_
#include "EntityComponents.h"

//nodes hold components only
//no logic occurs in a node

class RenderNode
{
public:
	RenderNode(std::shared_ptr<RenderComponent> _render, std::shared_ptr<TransformComponent> _transform) : render(_render), transform(_transform), selected(NULL) {};
	RenderNode(std::shared_ptr<RenderComponent> _render, std::shared_ptr<TransformComponent> _transform, std::shared_ptr<SelectedComponent> _selected) : render(_render), transform(_transform), selected(_selected) {};

	std::shared_ptr<RenderComponent> render;
	std::shared_ptr<TransformComponent> transform;
	std::shared_ptr<SelectedComponent> selected;
};

class MovableNode
{
public:
	MovableNode(std::shared_ptr<MovableComponent> _movable, std::shared_ptr<TransformComponent> _transform) : movable(_movable), transform(_transform) {};

	std::shared_ptr<MovableComponent> movable;
	std::shared_ptr<TransformComponent> transform;
};

class TransformNode
{
public:
	TransformNode(std::shared_ptr<TransformComponent> _transform) : transform(_transform) {};

	std::shared_ptr<TransformComponent> transform;
};

class SphereCollisionNode
{
public:
	SphereCollisionNode(std::shared_ptr<SphereCollsionComponent> _sCollision, std::shared_ptr<TransformComponent> _transform) : sCollision(_sCollision), movable(NULL), transform(_transform){};
	SphereCollisionNode(std::shared_ptr<SphereCollsionComponent> _sCollision, std::shared_ptr<TransformComponent> _transform, std::shared_ptr<MovableComponent> _movable) : sCollision(_sCollision), movable(_movable), transform(_transform){};

	std::shared_ptr<SphereCollsionComponent> sCollision;
	std::shared_ptr<MovableComponent> movable;
	std::shared_ptr<TransformComponent> transform;
};

class AABBCollisionNode
{
public:
	AABBCollisionNode(std::shared_ptr<AABBCollisionComponent> _aabbCollision, std::shared_ptr<TransformComponent> _transform) : aabbCollision(_aabbCollision), movable(NULL), transform(_transform){};
	AABBCollisionNode(std::shared_ptr<AABBCollisionComponent> _aabbCollision, std::shared_ptr<TransformComponent> _transform, std::shared_ptr<MovableComponent> _movable) : aabbCollision(_aabbCollision), movable(_movable), transform(_transform){};

	std::shared_ptr<AABBCollisionComponent> aabbCollision;
	std::shared_ptr<MovableComponent> movable;
	std::shared_ptr<TransformComponent> transform;
};

class FlexRigidNode
{

public:
	FlexRigidNode(std::shared_ptr<FlexBaseComponent> _flexBase, std::shared_ptr<FlexRigidComponent> _flexRigid) : flexBase(_flexBase), flexRigid(_flexRigid) {};
	FlexRigidNode(std::shared_ptr<RenderComponent> _render, std::shared_ptr<FlexBaseComponent> _flexBase, std::shared_ptr<FlexRigidComponent> _flexRigid) : render(_render), flexBase(_flexBase), flexRigid(_flexRigid) {};

	std::shared_ptr<RenderComponent> render;
	std::shared_ptr<FlexBaseComponent> flexBase;
	std::shared_ptr<FlexRigidComponent> flexRigid;

};

class FlexClothNode
{

public:
	FlexClothNode(std::shared_ptr<FlexBaseComponent> _flexBase, std::shared_ptr<FlexClothComponent> _flexCloth) : flexBase(_flexBase), flexCloth(_flexCloth) {};
	FlexClothNode(std::shared_ptr<RenderComponent> _render, std::shared_ptr<FlexBaseComponent> _flexBase, std::shared_ptr<FlexClothComponent> _flexCloth) : render(_render), flexBase(_flexBase), flexCloth(_flexCloth) {};

	std::shared_ptr<RenderComponent> render;
	std::shared_ptr<FlexBaseComponent> flexBase;
	std::shared_ptr<FlexClothComponent> flexCloth;
};

#endif