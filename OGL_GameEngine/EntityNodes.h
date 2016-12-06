#ifndef _ENTITY_NODES_H_
#define _ENTITY_NODES_H_
#include "EntityComponents.h"

//nodes hold components only
//no logic occurs in a node

class RenderNode
{
public:
	RenderNode(RenderComponent* _render, TransformComponent* _transform) : render(_render), transform(_transform), selected(NULL) {};
	RenderNode(RenderComponent* _render, TransformComponent* _transform, SelectedComponent* _selected) : render(_render), transform(_transform), selected(_selected) {};

	RenderComponent* render;
	TransformComponent* transform;
	SelectedComponent* selected;
};

class MovableNode
{
public:
	MovableNode(MovableComponent* _movable, TransformComponent* _transform) : movable(_movable), transform(_transform) {};

	MovableComponent* movable;
	TransformComponent* transform;
};

class TransformNode
{
public:
	TransformNode(TransformComponent* _transform) : transform(_transform) {};

	TransformComponent* transform;
};

class SphereCollisionNode
{
public:
	SphereCollisionNode(SphereCollsionComponent* _sCollision, TransformComponent* _transform) : sCollision(_sCollision), movable(NULL), transform(_transform){};
	SphereCollisionNode(SphereCollsionComponent* _sCollision, TransformComponent* _transform, MovableComponent* _movable) : sCollision(_sCollision), movable(_movable), transform(_transform){};

	SphereCollsionComponent* sCollision;
	MovableComponent* movable;
	TransformComponent* transform;
};

class AABBCollisionNode
{
public:
	AABBCollisionNode(AABBCollisionComponent* _aabbCollision, TransformComponent* _transform) : aabbCollision(_aabbCollision), movable(NULL), transform(_transform){};
	AABBCollisionNode(AABBCollisionComponent* _aabbCollision, TransformComponent* _transform, MovableComponent* _movable) : aabbCollision(_aabbCollision), movable(_movable), transform(_transform){};

	AABBCollisionComponent* aabbCollision;
	MovableComponent* movable;
	TransformComponent* transform;
};

#endif