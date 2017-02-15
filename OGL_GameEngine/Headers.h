#pragma once
#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <SDL.h>	//window creation
#include "GL/glew.h"	//opengl glew
#include <iostream>
#include <glm.hpp> // This is the main GLM header
#include <gtc/matrix_transform.hpp> // matrix transformations
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <vector>
#include <sstream>
#include <string>
#include <SOIL.h>	//header for file loading
#include <list>
#include <assimp/Importer.hpp>	//assimp is a model loader
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "flex/core/types.h"
#include "flex/core/maths.h"
#include "flex/core/platform.h"
//#include "flex/core/shader.h"
#include "flex/core/mesh.h"
#include "flex/core/voxelize.h"
#include "flex/core/sdf.h"
#include "flex/core/pfm.h"
#include "flex/core/tga.h"
#include "flex/core/perlin.h"
#include "flex/core/convex.h"
#include "flex/core/cloth.h"

#include "flex/include/flex.h"
#include "flex/include/flexExt.h"

//cuda stuff
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 1280
#define WINHEIGHT 720
#define FRAMERATE 60
#define PI 3.14159265359
#define HALF_PI 1.57079632679
#define GAMMA_CORRECTION 2.2

struct PointLight {
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

/* THIS ERROR CHECKER SAVED MY SANITY -- OPENGL DEBUGGING FUNCTION
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