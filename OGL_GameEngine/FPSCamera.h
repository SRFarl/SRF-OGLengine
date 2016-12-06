#pragma once
#ifndef _FPSCAMERA_H_
#define _FPSCAMERA_H_

#include "Headers.h"

class FPSCamera
{
	private:
		glm::mat4 view;		//viewing matrix for the camera
		glm::mat4 proj;		//projection matrix for the camera

		glm::vec3 eye;		//LookAt variables
		glm::vec3 centre;	
		glm::vec3 up;

		glm::vec3 rotationAngles;	//XYZ rotations about the init

		float m_cameraSpeed;		//speed that the camera moves
		GLfloat m_sensitivity;		//panning sensitivity

	public:
		FPSCamera(glm::vec3 initPos, glm::vec3 initDir, float sensitivity);
		
		void UpdateCamera(float deltaTs, bool forward, bool backward, bool left, bool right, bool leftMouseDown, GLfloat mouseXRel, GLfloat mouseYRel);
		
		glm::vec3 CreateMouseRayFromCamera(float mouseXPos, float mouseYPos);
		inline glm::vec3 GetPos() { return eye; }
		inline glm::mat4* GetView() { return &view; };
		inline glm::mat4* GetProj() { return &proj; };

};

#endif