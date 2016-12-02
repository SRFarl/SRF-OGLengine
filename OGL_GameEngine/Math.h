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

	float KinematicSphereStaticSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, glm::vec3 centerB, float radiusB)
	{
		// Find C, the vector from the center of the moving 
		// circle A to the center of B
		glm::vec3 C = centerB - centerA;
		float dirMag = glm::length(DirectionA);

		// Early Escape test : if the length of the DirectionA is less
		// than distance between the centers of these circles minus 
		// their radii, there's no way they can hit. 
		double dist = glm::length(C);
		double sumRadii = (radiusA + radiusB);
		dist -= sumRadii;
		if (dirMag < dist){
			return -1.0f;
		}

		// Normalize the DirectionA
		glm::vec3 N = glm::normalize(DirectionA);

		// D = N . C = ||C|| * cos(angle between N and C)
		double D = glm::dot(N,C);

		// Another early escape: Make sure that A is moving 
		// towards B! If the dot product between the DirectionA and 
		// B.center - A.center is less that or equal to 0, 
		// A isn't isn't moving towards B
		if (D <= 0){
			return -1.0f;
		}
		// Find the length of the vector C
		double lengthC = glm::length(C);

		double F = (lengthC * lengthC) - (D * D);

		// Escape test: if the closest that A will get to B 
		// is more than the sum of their radii, there's no 
		// way they are going collide
		double sumRadiiSquared = sumRadii * sumRadii;
		if (F >= sumRadiiSquared){
			return -1.0f;
		}

		// We now have F and sumRadii, two sides of a right triangle. 
		// Use these to find the third side, sqrt(T)
		double T = sumRadiiSquared - F;

		// If there is no such right triangle with sides length of 
		// sumRadii and sqrt(f), T will probably be less than 0. 
		// Better to check now than perform a square root of a 
		// negative number. 
		if (T < 0){
			return -1.0f;
		}

		// Therefore the distance the circle has to travel along 
		// DirectionA is D - sqrt(T)
		double distance = D - sqrt(T);

		// Finally, make sure that the distance A has to move 
		// to touch B is not greater than the magnitude of the 
		// movement vector. 
		if (dirMag < distance){
			return -1.0f;
		}

		return distance;
	}

	float KinematicSphereKinematicSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, float massA, glm::vec3 centerB, glm::vec3 DirectionB, float radiusB, float massB, glm::vec3 &resultA, glm::vec3 &resultB)
	{
		//colision
		glm::vec3 AB = (centerA + DirectionA) - (centerB + DirectionB);
		if (glm::length(AB) > (radiusA + radiusB))
		{
			return false;
		}

		//bounce http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2
		glm::vec3 n = glm::normalize(centerA - centerB);

		float a1 = glm::dot(DirectionA, n);
		float a2 = glm::dot(DirectionB, n);

		float optimizedP = (2.0 * (a1 - a2)) / (massA + massB);

		glm::vec3 v1b = DirectionA - optimizedP * massB * n;
		glm::vec3 v2b = DirectionB + optimizedP * massA * n;

		resultA = v1b;
		resultB = v2b;

		return true;
	}

}

#endif