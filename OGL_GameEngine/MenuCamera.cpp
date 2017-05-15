#include "MenuCamera.h"

MenuCamera::MenuCamera(glm::vec3 initPos, glm::vec3 initDir, float sensitivity) : Camera(initPos, initDir), m_cameraSpeed(10.0f), m_sensitivity(sensitivity), rotationAroundCentre(0.0f), radiusFromCentre(initPos.x), rotationAngles(initDir)
{
}

void MenuCamera::UpdateCamera(float deltaTs)
{
	//moving around the origin
	eyeWithRot = eye;

	rotationAngles.x -= m_cameraSpeed * deltaTs;
	rotationAroundCentre -= m_cameraSpeed * deltaTs;

	//update the viewing direction based on the rotations of the camera
	glm::vec3 centreTemp;
	centreTemp.x = glm::cos(glm::radians(rotationAngles.y)) * glm::cos(glm::radians(rotationAngles.x));
	centreTemp.y = glm::sin(glm::radians(rotationAngles.y));
	centreTemp.z = glm::cos(glm::radians(rotationAngles.y)) * glm::sin(glm::radians(rotationAngles.x));
	centre = glm::normalize(centreTemp);

	eyeWithRot.x = radiusFromCentre * glm::cos(glm::radians(rotationAroundCentre));
	eyeWithRot.z = radiusFromCentre * glm::sin(glm::radians(rotationAroundCentre));

	//update the viewing matrix
	view = glm::lookAt(eyeWithRot, eyeWithRot + centre, up);
}
