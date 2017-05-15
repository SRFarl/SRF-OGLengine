#ifndef _MENUCAMERA_H_
#define _MENUCAMERA_H_

#include "Headers.h"
#include "Camera.h"

class MenuCamera : public Camera
{
public:
	MenuCamera(glm::vec3 initPos, glm::vec3 initDir, float sensitivity);

	void UpdateCamera(float deltaTs);

private:
	glm::vec3 rotationAngles;	//XYZ rotations about the init
	glm::vec3 eyeWithRot; //real position with rotation
	float rotationAroundCentre;		//current rotation around the center point of the world
	float radiusFromCentre;	

	float m_cameraSpeed;		//speed that the camera moves
	GLfloat m_sensitivity;		//panning sensitivity
};

#endif