#pragma once
#ifndef _MATH_H_
#define _MATH_H_

#include <glm.hpp> // This is the main GLM header
#include <gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <gtc/type_ptr.hpp>

namespace Math
{
	const float almostZero = 0.00001f;

	bool RayTriIntersect(
				const glm::vec3 &orig, const glm::vec3 &dir,
				const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
	{
		//http://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates
		
		//compute normal
		glm::vec3 v0v1 = v1 - v0;
		glm::vec3 v0v2 = v2 - v0;

		glm::vec3 normal = glm::cross(v0v1, v0v2);

		//find P, point of intersect

		//check if parallel
		float normalDotDirection = glm::dot(normal, dir);
		if (fabs(normalDotDirection) < almostZero)
			return false;

		//equation 2
		float d = glm::dot(normal, v0);

		//equation 3 // ADDED 2 NEGATIVES BECAUSE SITES EXAMPLE DOESN'T MAKE SENSE //
		float t = -(glm::dot(normal, orig) - d) / normalDotDirection;
		//check if triangle is behind ray
		if (t < 0) 
			return false;

		//find intersection P
		glm::vec3 P = orig + t * dir;

		//inside-outside test
		glm::vec3 C;	//perpendicular to tri plane

		//edge 0
		glm::vec3 edge0 = v1 - v0;
		glm::vec3 vp0 = P - v0;
		C = glm::cross(edge0, vp0);
		if (glm::dot(normal, C) < 0)
			return false;

		//edge 1
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 vp1 = P - v1;
		C = glm::cross(edge1, vp1);
		if (glm::dot(normal, C) < 0)
			return false;

		//edge 2
		glm::vec3 edge2 = v0 - v2;
		glm::vec3 vp2 = P - v2;
		C = glm::cross(edge2, vp2);
		if (glm::dot(normal, C) < 0)
			return false;

		return true;
	}

	glm::vec3 FindNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		glm::vec3 v0v1 = v1 - v0;
		glm::vec3 v0v2 = v2 - v0;

		return glm::normalize(glm::cross(v0v1, v0v2));
	}
}

#endif