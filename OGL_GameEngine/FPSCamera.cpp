#include "FPSCamera.h"

//////////////////////////////////////////////////////////////////////////
//Camera Constructor
//////////////////////////////////////////////////////////////////////////
FPSCamera::FPSCamera(float sensitivity) : m_cameraSpeed(3.0f), m_sensitivity(sensitivity)
{
	eye = glm::vec3(0,0,-3.0f);	//position of camera
	centre = glm::vec3(0,0,1);	//viewing direction when added to the eye
	up = glm::vec3(0,1,0);		//up direction vector

	//initialise variables
	rotationAngles = glm::vec3(90,0,0);
	m_lmouseX = WINWIDTH/2;
	m_lmouseY = WINHEIGHT/2;

	//init proj mat
	proj = glm::perspective(45.0f, ((float)WINWIDTH / (float)WINHEIGHT), 0.1f, 100.0f);

	view = glm::lookAt(eye, eye + centre, up);
}

//////////////////////////////////////////////////////////////////////////
//Update for the cameras movement and direction
//////////////////////////////////////////////////////////////////////////
void FPSCamera::UpdateCamera(float deltaTs, bool forward, bool backward, bool left, bool right, GLfloat mouseXRel, GLfloat mouseYRel)
{
	mouseXRel *= (m_sensitivity * deltaTs);
	mouseYRel *= (m_sensitivity * deltaTs);

	rotationAngles.x += mouseXRel;	//yaw
	rotationAngles.y -= mouseYRel;	//pitch

	if (rotationAngles.y > 89.0f)
		rotationAngles.y = 89.0f;
	if (rotationAngles.y < -89.0f)
		rotationAngles.y = -89.0f;

	//update the viewing direction based on the rotations of the camera
	glm::vec3 centreTemp;
	centreTemp.x = glm::cos(glm::radians(rotationAngles.y)) * glm::cos(glm::radians(rotationAngles.x));
	centreTemp.y = glm::sin(glm::radians(rotationAngles.y));
	centreTemp.z = glm::cos(glm::radians(rotationAngles.y)) * glm::sin(glm::radians(rotationAngles.x));
	centre = glm::normalize(centreTemp);
	//key movement. based on viewing direction
	if(forward)
	{
		eye.z += centre.z * m_cameraSpeed * deltaTs;
		eye.x += centre.x * m_cameraSpeed * deltaTs;
		eye.y += centre.y * m_cameraSpeed * deltaTs;
	}
	if(backward)
	{
		eye.z -= centre.z * m_cameraSpeed * deltaTs;
		eye.x -= centre.x * m_cameraSpeed * deltaTs;
		eye.y -= centre.y * m_cameraSpeed * deltaTs;
	}
	if(left)
	{
		eye -= glm::normalize(glm::cross(centre, up)) *m_cameraSpeed * deltaTs;
	}
	if (right)
	{
		eye += glm::normalize(glm::cross(centre, up)) *m_cameraSpeed * deltaTs;
	}

	//update the viewing matrix
	view = glm::lookAt(eye, eye + centre, up);

}
