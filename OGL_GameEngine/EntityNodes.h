#ifndef _ENTITY_NODES_H_
#define _ENTITY_NODES_H_
#include "EntityComponents.h"

class RenderNode
{
public:
	RenderNode(RenderComponent* _render, TransformComponent* _transform) : render(_render), transform(_transform){};

	RenderComponent* render;
	TransformComponent* transform;
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

#endif