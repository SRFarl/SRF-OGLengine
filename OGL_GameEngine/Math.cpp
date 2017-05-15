#include "Math.h"

bool Math::RayTriIntersect(const glm::vec3 &orig, const glm::vec3 &dir, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2)
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

glm::vec3 Math::FindNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;

	return glm::normalize(glm::cross(v0v1, v0v2));
}

float Math::VectorSum(glm::vec3 _in)
{
	return _in.x + _in.y + _in.z;
}

bool Math::RaySphereIntersect(glm::vec3 raydir, glm::vec3 rayorig, glm::vec3 pos, float rad)
{
	//http://sci.tuomastonteri.fi/programming/sse/example3

	float a = VectorSum(raydir*raydir);
	float b = VectorSum(raydir * (2.0f * (rayorig - pos)));
	float c = VectorSum(pos*pos) + VectorSum(rayorig*rayorig) - 2.0f*VectorSum(rayorig*pos) - rad*rad;
	float D = b*b + (-4.0f)*a*c;

	// If ray can not intersect then stop
	if (D < 0)
		return false;
	D = sqrtf(D);

	// Ray can intersect the sphere, solve the closer hitpoint
	float t = (-0.5f)*(b + D) / a;
	if (t > 0.0f)
	{
		return true;
	}
	else
		return false;
}

float Math::KinematicSphereStaticSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, glm::vec3 centerB, float radiusB)
{
	//http://www.gamasutra.com/view/feature/3015/pool_hall_lessons_fast_accurate_.php

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
	if (dirMag < dist) {
		return -1.0f;
	}

	// Normalize the DirectionA
	glm::vec3 N = glm::normalize(DirectionA);

	// D = N . C = ||C|| * cos(angle between N and C)
	double D = glm::dot(N, C);

	// Another early escape: Make sure that A is moving 
	// towards B! If the dot product between the DirectionA and 
	// B.center - A.center is less that or equal to 0, 
	// A isn't isn't moving towards B
	if (D <= 0) {
		return -1.0f;
	}
	// Find the length of the vector C
	double lengthC = glm::length(C);

	double F = (lengthC * lengthC) - (D * D);

	// Escape test: if the closest that A will get to B 
	// is more than the sum of their radii, there's no 
	// way they are going collide
	double sumRadiiSquared = sumRadii * sumRadii;
	if (F >= sumRadiiSquared) {
		return -1.0f;
	}

	// We now have F and sumRadii, two sides of a right triangle. 
	// Use these to find the third side, sqrt(T)
	double T = sumRadiiSquared - F;

	// If there is no such right triangle with sides length of 
	// sumRadii and sqrt(f), T will probably be less than 0. 
	// Better to check now than perform a square root of a 
	// negative number. 
	if (T < 0) {
		return -1.0f;
	}

	// Therefore the distance the circle has to travel along 
	// DirectionA is D - sqrt(T)
	double distance = D - sqrt(T);

	// Finally, make sure that the distance A has to move 
	// to touch B is not greater than the magnitude of the 
	// movement vector. 
	if (dirMag < distance) {
		return -1.0f;
	}

	return distance;
}

float Math::KinematicSphereKinematicSphereCollision(glm::vec3 centerA, glm::vec3 DirectionA, float radiusA, float massA, glm::vec3 centerB, glm::vec3 DirectionB, float radiusB, float massB, glm::vec3 &resultA, glm::vec3 &resultB)
{
	//colision
	glm::vec3 AB = (centerA + DirectionA) - (centerB + DirectionB);
	if (glm::length(AB) >= (radiusA + radiusB))
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

bool Math::AABBIntersection(glm::vec3 minA, glm::vec3 maxA, glm::vec3 minB, glm::vec3 maxB)
{
	return (minA.x <= maxB.x && maxA.x >= minB.x) &&
		(minA.y <= maxB.y && maxA.y >= minB.y) &&
		(minA.z <= maxB.z && maxA.z >= minB.z);
}

float Math::KinematicAABBStaticAABB(glm::vec3 minA, glm::vec3 maxA, glm::vec3 DirectionA, glm::vec3 minB, glm::vec3 maxB, glm::vec3 &normal)
{
	//http://www.gamedev.net/page/resources/_/technical/game-programming/swept-aabb-collision-detection-and-response-r3084

	//early out intersection test
	if (!AABBIntersection(minA + DirectionA, maxA + DirectionA, minB, maxB))
		return -1.0f;

	//find distance and time it takes to reach a collision
	glm::vec3 InvEntry; //closest edges distance
	glm::vec3 InvExit;	//distance to furthest side of the object

	if (DirectionA.x > 0)
	{
		InvEntry.x = minB.x - maxA.x;
		InvExit.x = maxB.x - minA.x;
	}
	else
	{
		InvEntry.x = minA.x - maxB.x;
		InvExit.x = maxA.x - minB.x;
	}
	if (DirectionA.y > 0)
	{
		InvEntry.y = minB.y - maxA.y;
		InvExit.y = maxB.y - minA.y;
	}
	else
	{
		InvEntry.y = minA.y - maxB.y;
		InvExit.y = maxA.y - minB.y;
	}
	if (DirectionA.z > 0)
	{
		InvEntry.z = minB.z - maxA.z;
		InvExit.z = maxB.z - minA.z;
	}
	else
	{
		InvEntry.z = minA.z - maxB.z;
		InvExit.z = maxA.z - minB.z;
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
	glm::vec3 entry;
	glm::vec3 exit;

	if (DirectionA.x == 0.0f)
	{
		entry.x = -std::numeric_limits<float>::infinity();
		exit.x = std::numeric_limits<float>::infinity();
	}
	else
	{
		entry.x = InvEntry.x / fabs(DirectionA.x);
		exit.x = InvExit.x / fabs(DirectionA.x);
	}

	if (DirectionA.y == 0.0f)
	{
		entry.y = -std::numeric_limits<float>::infinity();
		exit.y = std::numeric_limits<float>::infinity();
	}
	else
	{
		entry.y = InvEntry.y / fabs(DirectionA.y);
		exit.y = InvExit.y / fabs(DirectionA.y);
	}

	if (DirectionA.z == 0.0f)
	{
		entry.z = -std::numeric_limits<float>::infinity();
		exit.z = std::numeric_limits<float>::infinity();
	}
	else
	{
		entry.z = InvEntry.y / fabs(DirectionA.z);
		exit.z = InvExit.y / fabs(DirectionA.z);
	}

	// find the earliest/latest times of collision
	float entryTime = std::max(std::max(entry.x, entry.y), entry.z);
	float exitTime = std::max(std::max(exit.x, exit.y), exit.z);

	// second intersection test
	if (entryTime > exitTime || entry.x < 0.0f && entry.y < 0.0f && entry.z < 0.0f || entry.x > 1.0f || entry.y > 1.0f || entry.z > 1.0f)
	{
		return -1.0f;
	}
	else // if there was a collision
	{
		// calculate normal of collided surface
		if (entry.x > entry.y && entry.x > entry.z)
		{
			if (InvEntry.x > 0.0f)
			{
				normal.x = 1.0f;
				normal.y = 0.0f;
				normal.z = 0.0f;
			}
			else
			{
				normal.x = -1.0f;
				normal.y = 0.0f;
				normal.z = 0.0f;
			}
		}
		else if (entry.y > entry.x && entry.y > entry.z)
		{
			if (InvEntry.y > 0.0f)
			{
				normal.x = 0.0f;
				normal.y = 1.0f;
				normal.z = 0.0f;
			}
			else
			{
				normal.x = 0.0f;
				normal.y = -1.0f;
				normal.z = 0.0f;
			}
		}
		else
		{
			if (InvEntry.z > 0.0f)
			{
				normal.x = 0.0f;
				normal.y = 0.0f;
				normal.z = 1.0f;
			}
			else
			{
				normal.x = 0.0f;
				normal.y = 0.0f;
				normal.z = -1.0f;
			}
		}
		// return the time of collision
		return entryTime;
	}

	return 0;
}

float Math::KinematicAABBKinematicAABBCollision(glm::vec3 centerA, glm::vec3 DirectionA, glm::vec3 minA, glm::vec3 maxA, float massA, glm::vec3 centerB, glm::vec3 DirectionB, glm::vec3 minB, glm::vec3 maxB, float massB, glm::vec3 &resultA, glm::vec3 &resultB)
{
	//colision
	if (!AABBIntersection(minA + DirectionA, maxA + DirectionA, minB + DirectionB, maxB + DirectionB))
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

float Math::SquaredDistanceFromPointToClosestAABBPoint(glm::vec3 & p, glm::vec3 minA, glm::vec3 maxA)
{
	float sq = 0.0;

	sq += PointToClosestAABBPointAxisCheck(p.x, minA.x, maxA.x);
	sq += PointToClosestAABBPointAxisCheck(p.y, minA.y, maxA.y);
	sq += PointToClosestAABBPointAxisCheck(p.z, minA.z, maxA.z);

	return sq;
}

float Math::PointToClosestAABBPointAxisCheck(float pn, float bmin, float bmax)
{
	float out = 0;
	float v = pn;

	if (v < bmin)
	{
		float val = (bmin - v);
		out += val * val;
	}

	if (v > bmax)
	{
		float val = (v - bmax);
		out += val * val;
	}

	return out;
}

bool Math::SphereAABBIntersection(glm::vec3 sphereCentre, float sphereRadius, glm::vec3 minA, glm::vec3 maxA)
{
	float squaredDistance = SquaredDistanceFromPointToClosestAABBPoint(sphereCentre, minA, maxA);

	// Intersection if the distance from center is larger than the radius
	// of the sphere.

	return squaredDistance <= (sphereRadius * sphereRadius);
}