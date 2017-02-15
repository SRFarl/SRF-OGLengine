#pragma once
#ifndef _OBJLOADER_SRF_H_
#define _OBJLOADER_SRF_H_

#include "Headers.h"
#include <fstream>

namespace obj
{
	bool Load_OBJ(const char* filename, std::vector<glm::vec4> &vertices, std::vector<glm::vec2> &UVs, std::vector<glm::vec3> &normals, unsigned int &numOfVertexes);
};





#endif