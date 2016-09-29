#pragma once
#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <SDL.h>
#include "GL/glew.h"
#include <iostream>
#include <glm.hpp> // This is the main GLM header
#include <gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <SOIL.h>
#include <list>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 1280
#define WINHEIGHT 720
#define FRAMERATE 60
#define PI 3.14159265359
#define HALF_PI 1.57079632679

class RenderComponent;
class RenderNode;

struct PointLight {
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
	
	RenderComponent* m_rComp;
	RenderNode* m_renderNode;
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

/* THIS ERROR CHECKER SAVED MY SANITY 
void _CheckGLError(const char* file, int line);

#define CheckGLError() _CheckGLError(__FILE__, __LINE__)

void _CheckGLError(const char* file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;
		switch (err)
		{
		case GL_INVALID_OPERATION:  error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:       error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:      error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:      error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}
		std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
		err = glGetError();
	}

	return;
}
*/

#endif 