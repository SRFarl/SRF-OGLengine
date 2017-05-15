#ifndef _FPSCAMERA_H_
#define _FPSCAMERA_H_

#include "Headers.h"
#include "Camera.h"

class FPSCamera : public Camera
{
private:
	glm::vec3 rotationAngles;	//XYZ rotations about the init

	float m_cameraSpeed;		//speed that the camera moves
	GLfloat m_sensitivity;		//panning sensitivity

public:
	FPSCamera(glm::vec3 initPos, glm::vec3 initDir, float sensitivity);

	void UpdateCamera(float deltaTs, bool forward, bool backward, bool left, bool right, bool leftMouseDown, GLfloat mouseXRel, GLfloat mouseYRel);

	glm::vec3 CreateMouseRayFromCamera(float mouseXPos, float mouseYPos);
};

#endif