#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Headers.h"

class Camera
{
public:
	Camera(glm::vec3 initPos, glm::vec3 initDir);

	inline glm::vec3 GetPos() { return eye; }
	inline glm::mat4* GetView() { return &view; };
	inline glm::mat4* GetProj() { return &proj; };
	inline glm::vec3* GetDirection() { return &centre; };

	glm::mat4 view;		//viewing matrix for the camera
	glm::mat4 proj;		//projection matrix for the camera

	glm::vec3 eye;		//LookAt variables
	glm::vec3 centre;
	glm::vec3 up;
};

#endif