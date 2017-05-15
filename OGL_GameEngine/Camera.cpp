#include "Camera.h"

Camera::Camera(glm::vec3 initPos, glm::vec3 initDir)
{
	eye = initPos;	//position of camera

	glm::vec3 centreTemp;
	centreTemp.x = glm::cos(glm::radians(initDir.y)) * glm::cos(glm::radians(initDir.x));
	centreTemp.y = glm::sin(glm::radians(initDir.y));
	centreTemp.z = glm::cos(glm::radians(initDir.y)) * glm::sin(glm::radians(initDir.x));
	centre = glm::normalize(centreTemp);

	up = glm::vec3(0, 1, 0);		//up direction vector

	//init proj mat
	proj = glm::perspective(45.0f, ((float)WINWIDTH / (float)WINHEIGHT), 0.1f, 100.0f);

	//build view matrix
	view = glm::lookAt(eye, eye + centre, up);
}
