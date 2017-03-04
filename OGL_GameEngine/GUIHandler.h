#ifndef _GUIHANDLER_H_
#define _GUIHANDLER_H_

#include "Headers.h"
#include "Shader.h"
#include "Model.h"

struct Sprite
{
	std::string name;
	GLuint textureId;
	glm::vec2 position;
	GLfloat rotate;
	glm::vec2 size;
	bool active;
};

class GUIHandler 
{
public:
	GUIHandler(std::string vertexPath, std::string fragPath)
	{
		//init projection
		m_guiProj = glm::ortho(0.0f, (float) WINWIDTH, (float) WINHEIGHT, 0.0f, -1.0f, 1.0f);

		m_guiShader = new Shader("gui", vertexPath.c_str(), fragPath.c_str());

		//create triangles
		GLfloat vertices[] = {
			// Pos      // Tex
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};

		GLuint VBO;
		glGenVertexArrays(1, &squareVAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(squareVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	~GUIHandler()
	{
		delete m_guiShader;
		glDeleteVertexArrays(1, &squareVAO);
		m_spriteList.clear();
	}

	void DeleteSprite(std::string _name)
	{
		for (std::vector<Sprite>::iterator it = m_spriteList.begin(); it != m_spriteList.end();)
		{
			if (it->name == _name)
			{
				m_spriteList.erase(it);
				return;
			}

			it++;
		}
	}

	Sprite* GetSprite(std::string _name)
	{
		for (std::vector<Sprite>::iterator it = m_spriteList.begin(); it != m_spriteList.end();)
		{
			if (it->name == _name)
			{
				return &(*it);
			}
		}

		return NULL;
	}

	void LoadSprite(std::string spriteName, std::string spriteFileName, std::string spriteDirectory, glm::vec2 initPos, GLfloat initRotate, glm::vec2 initSize)
	{
		Sprite temp;
		temp.name = spriteName;
		temp.textureId = TextureFromFile(spriteFileName.c_str(), spriteDirectory);
		temp.position = initPos;
		temp.rotate = initRotate;
		temp.size = initSize;
		temp.active = true;

		m_spriteList.push_back(temp);
	}

	void DrawGUI()
	{
		glDisable(GL_DEPTH_TEST);

		for (std::vector<Sprite>::iterator it = m_spriteList.begin(); it != m_spriteList.end();)
		{
			if (it->active)
			{
				m_guiShader->UseShader();
				glm::mat4 model;

				//translate, rotate, scale due to right to left matrix multiplication
				//move to position
				model = glm::translate(model, glm::vec3(it->position, 0.0f));

				//change pivot to center of sprite
				model = glm::translate(model, glm::vec3(0.5f * it->size.x, 0.5f * it->size.y, 0.0f));

				//rotate
				model = glm::rotate(model, it->rotate, glm::vec3(0.0f, 0.0f, 1.0f));

				//change pivot back to top left corner
				model = glm::translate(model, glm::vec3(-0.5f * it->size.x, -0.5f * it->size.y, 0.0f));

				//scale
				model = glm::scale(model, glm::vec3(it->size, 1.0f));

				glUniformMatrix4fv(glGetUniformLocation(m_guiShader->getShaderProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(glGetUniformLocation(m_guiShader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(m_guiProj));

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(glGetUniformLocation(m_guiShader->getShaderProgram(), "image"), 0);
				glBindTexture(GL_TEXTURE_2D, it->textureId);

				glBindVertexArray(squareVAO);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			++it;
		}

		glEnable(GL_DEPTH_TEST);
	}
private:
	glm::mat4 m_guiProj;		//ortho projection matrix for the camera
	Shader *m_guiShader;
	std::vector<Sprite> m_spriteList;
	GLuint squareVAO;
};


#endif