#ifndef _ENTITY_NODES_H_
#define _ENTITY_NODES_H_
#include "EntityComponents.h"

class RenderNode
{
public:
	RenderNode(glm::mat4* _position, RenderComponent* _render) : position(_position), render(_render){};

	glm::mat4* position;
	RenderComponent* render;
};

class MovableNode
{
public:
	MovableNode(glm::vec3* _position, MovableComponent* _movable) : position(_position), movable(_movable) {};

	glm::vec3* position;
	MovableComponent* movable;
};

class RotationNode
{
public:
	RotationNode(RotationComponent* _rotation) : rotation(_rotation) {};

	RotationComponent* rotation;
};

#endif