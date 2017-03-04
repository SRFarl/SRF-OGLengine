#ifndef _GUIHANDLER_H_
#define _GUIHANDLER_H_

#include "Headers.h"
#include "Shader.h"
#include "Model.h"
#include <ft2build.h>
#include FT_FREETYPE_H 

//https://learnopengl.com/#!In-Practice/Text-Rendering

struct Sprite
{
	std::string name;
	GLuint textureId;
	glm::vec2 position;
	GLfloat rotate;
	glm::vec2 size;
	bool active;
};

struct Character {
	GLuint     textureId;  // ID handle of the glyph texture
	glm::ivec2 size;       // Size of glyph
	glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
	GLuint     advance;    // Offset to advance to next glyph
};

class GUIHandler 
{
public:
	GUIHandler(std::string guiVertexPath, std::string guiFragPath, std::string fontVertexPath, std::string fontFragPath, int _fontSize);

	~GUIHandler();

	void DeleteSprite(std::string _name);

	Sprite* GetSprite(std::string _name);

	void LoadSprite(std::string spriteName, std::string spriteFileName, std::string spriteDirectory, glm::vec2 initPos, GLfloat initRotate, glm::vec2 initSize);

	void DrawGUI();

	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	glm::mat4 m_guiProj;		//ortho projection matrix for the camera
	Shader *m_guiShader;
	std::vector<Sprite> m_spriteList;
	GLuint squareVAO;

private:
	//font
	glm::mat4 m_fontProj;
	Shader *m_fontShader;
	GLuint m_fontVAO;
	GLuint m_fontVBO;
	std::map<GLchar, Character> m_characters;
};


#endif