#ifndef _SHADER_H_
#define _SHADER_H_

#include "Headers.h"
#include <fstream>

class Shader
{
public:
	Shader(std::string shaderName, const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
	~Shader();
	void UseShader() { if (m_bisShaderOkay)glUseProgram(m_glprogram); }
	bool getIsShaderOkay() { return m_bisShaderOkay; }
	GLuint getShaderProgram() { return m_glprogram; }

private:
	bool m_bisShaderOkay;
	std::string m_strshaderName;
	GLuint m_glprogram;
};

#endif