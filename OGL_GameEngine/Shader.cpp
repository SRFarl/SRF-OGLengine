#include "Shader.h"

Shader::Shader(std::string shaderName, const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath) : m_strshaderName(shaderName)
{
	//FILE LOADING ONLY TAKEN FROM http://www.learnopengl.com/#!Getting-started/Shaders

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::badbit);

	try
	{
		//Open files
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);

		std::stringstream vertexShaderStream;
		std::stringstream fragmentShaderStream;

		//read in files to streams
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		//close files
		vertexShaderFile.close();
		fragmentShaderFile.close();

		//convert to string
		vertexCode = vertexShaderStream.str();
		fragmentCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		m_bisShaderOkay = false;
		return;
	}

	//convert to GLchar
	const GLchar* vertexShaderCode = vertexCode.c_str();
	const GLchar* fragmentShaderCode = fragmentCode.c_str();

	//////////////////////////////////////////////////////////////////////////

	//CREATE THE SHADER PROGRAM
	m_glprogram = glCreateProgram();

	//VERTEX
	GLuint nVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(nVertShader, 1, &vertexShaderCode, NULL);
	glCompileShader(nVertShader);

	//error checking
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(nVertShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if (vShaderCompiled != GL_TRUE)
	{
		GLsizei len;
		glGetShaderiv(nVertShader, GL_INFO_LOG_LENGTH, &len);

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len + 1];
		glGetShaderInfoLog(nVertShader, len, &len, log);
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete[] log;

		glDeleteShader(nVertShader);
		m_bisShaderOkay = false;
		return;
	}
	else
	{
		glAttachShader(m_glprogram, nVertShader);
	}

	glDeleteShader(nVertShader);

	//FRAGMENT
	GLuint nFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(nFragShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(nFragShader);

	//error checking
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(nFragShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if (fShaderCompiled != GL_TRUE)
	{
		GLsizei len;
		glGetShaderiv(nFragShader, GL_INFO_LOG_LENGTH, &len);

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len + 1];
		glGetShaderInfoLog(nFragShader, len, &len, log);
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete[] log;

		glDeleteShader(nFragShader);
		m_bisShaderOkay = false;
		return;
	}
	else
	{
		glAttachShader(m_glprogram, nFragShader);
	}

	glDeleteShader(nFragShader);

	//LINK PROGRAM AND CHECK SUCCESS
	glLinkProgram(m_glprogram);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(m_glprogram, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("%s - Error linking program %d!\n", shaderName, m_glprogram);
		glDeleteProgram(m_glprogram);
		m_bisShaderOkay = false;
		return;
	}

	glUseProgram(m_glprogram);
	glUseProgram(0);

	m_bisShaderOkay = true;
}

Shader::~Shader()
{
	//delete the program if it's not null
	if (m_glprogram)
	{
		glDeleteProgram(m_glprogram);
	}
}

bool CheckShaderCompiled(GLint shader)
{
	//checks if the shader is compiled by looking at error logs
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		// OpenGL will store an error message as a string that we can retrieve and print
		GLchar* log = new GLchar[len + 1];
		glGetShaderInfoLog(shader, len, &len, log);
		std::cerr << "ERROR: Shader compilation failed: " << log << std::endl;
		delete[] log;

		return false;
	}
	return true;
}