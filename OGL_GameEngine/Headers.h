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

#define WINPOSX 100
#define WINPOSY 100
#define WINWIDTH 1280
#define WINHEIGHT 720
#define FRAMERATE 60
#define PI 3.14159265359
#define HALF_PI 1.57079632679

struct Material {
	GLuint diffuse;
	GLfloat diffuseSet;
	GLuint specular;
	GLfloat specularSet;
	GLuint emission;
	GLfloat emissionSet;
	GLfloat shininess;
};

struct Asset
{
	std::string assetName;
	GLuint m_glVertBuffer; //vertex VBO
	GLuint m_glNormBuffer; //normal VBO
	GLuint m_glUVBuffer; //uv VBO
	GLuint m_glIndexBuffer;
	GLuint m_glVertArray; //VAO
	Material m_glMaterial;
	GLsizei m_unVertexCount;
};

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

#endif 