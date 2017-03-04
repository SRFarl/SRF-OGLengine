#include "GUIHandler.h"

GUIHandler::GUIHandler(std::string guiVertexPath, std::string guiFragPath, std::string fontVertexPath, std::string fontFragPath, int _fontSize)
{
	//init projection
	m_guiProj = glm::ortho(0.0f, (float)WINWIDTH, (float)WINHEIGHT, 0.0f, -1.0f, 1.0f);
	m_fontProj = glm::ortho(0.0f, (float)WINWIDTH, 0.0f, (float)WINHEIGHT, -1.0f, 1.0f);

	//shaders
	m_guiShader = new Shader("gui", guiVertexPath.c_str(), guiFragPath.c_str());
	m_fontShader = new Shader("gui", fontVertexPath.c_str(), fontFragPath.c_str());

	//create gui square to render textures on
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

	//load font stuff
	FT_Library ftLibrary;
	FT_Face ftFace;

	if (FT_Init_FreeType(&ftLibrary))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(ftLibrary, "GUI/fonts/arial.ttf", 0, &ftFace))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	//font size
	FT_Set_Pixel_Sizes(ftFace, 0, _fontSize);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//load each character
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Now store character for later use
		Character character;

		character.textureId = texture;
		character.size = glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows);
		character.bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
		character.advance = ftFace->glyph->advance.x;

		m_characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLibrary);

	//create square to render fonts on
	glGenVertexArrays(1, &m_fontVAO);
	glGenBuffers(1, &m_fontVBO);
	glBindVertexArray(m_fontVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GUIHandler::~GUIHandler()
{
	delete m_guiShader;
	delete m_fontShader;
	glDeleteVertexArrays(1, &squareVAO);
	m_spriteList.clear();
}

void GUIHandler::DeleteSprite(std::string _name)
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

Sprite* GUIHandler::GetSprite(std::string _name)
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

void GUIHandler::LoadSprite(std::string spriteName, std::string spriteFileName, std::string spriteDirectory, glm::vec2 initPos, GLfloat initRotate, glm::vec2 initSize)
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

void GUIHandler::DrawGUI()
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

void GUIHandler::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour)
{
	glDisable(GL_DEPTH_TEST);

	// Activate corresponding render state	
	m_fontShader->UseShader();
	glUniformMatrix4fv(glGetUniformLocation(m_fontShader->getShaderProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(m_fontProj));
	glUniform3f(glGetUniformLocation(m_fontShader->getShaderProgram(), "textColour"), colour.x, colour.y, colour.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_fontVAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = m_characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glUniform1i(glGetUniformLocation(m_fontShader->getShaderProgram(), "text"), 0);
		glBindTexture(GL_TEXTURE_2D, ch.textureId);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}
