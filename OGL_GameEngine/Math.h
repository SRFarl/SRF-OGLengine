#pragma once
#ifndef _MATH_H_
#define _MATH_H_

#include <glm.hpp> // This is the main GLM header
#include <gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <gtc/type_ptr.hpp>
#include <algorithm>

namespace Math
{
	const float almostZero = 0.00001f;

	bool RayTriIntersect(
				const glm::vec3 &orig, const glm::vec3 &dir,
				const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2);

	glm::vec3 FindNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

	float VectorSum(glm::vec3 _in);

	bool RaySphereIntersect(glm::vec3 raydir, glm::vec3 rayorig, glm::vec3 pos,
		float rad);

	float KinematicSphereStaticSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, glm::vec3 centerB, float radiusB);

	float KinematicSphereKinematicSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, float massA, glm::vec3 centerB, glm::vec3 DirectionB, float radiusB, float massB, glm::vec3 &resultA, glm::vec3 &resultB);

	bool AABBIntersection(glm::vec3 minA, glm::vec3 maxA, glm::vec3 minB, glm::vec3 maxB);

	float KinematicAABBStaticAABB(glm::vec3 minA, glm::vec3 maxA, glm::vec3 DirectionA, glm::vec3 minB, glm::vec3 maxB, glm::vec3 &normal);

	float KinematicAABBKinematicAABBCollision(glm::vec3 centerA, glm::vec3 DirectionA, glm::vec3 minA, glm::vec3 maxA, float massA, glm::vec3 centerB, glm::vec3 DirectionB, glm::vec3 minB, glm::vec3 maxB, float massB, glm::vec3 &resultA, glm::vec3 &resultB);
}

#endif