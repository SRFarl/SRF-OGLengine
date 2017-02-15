#ifndef _MODELMESH_H_
#define _MODELMESH_H_
#include "Headers.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 VertexColour;

	glm::vec3 Tangent;
	glm::vec3 BiTangent;
};

struct Texture {
	GLuint id;
	std::string type;
};

class ModelMesh {
public:
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;

	ModelMesh(std::vector<Vertex>* _vertices, std::vector<GLuint>* _indices, std::vector<Texture>* _textures, bool dynamicDrawing) : m_vertices(*_vertices), m_indices(*_indices), m_textures(*_textures)
	{
		Setup(dynamicDrawing);
	};

	void Draw(GLuint program)
	{
		// Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		GLuint normalNr = 1;
		for (GLuint i = 0; i < this->m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
			// Retrieve texture number (the N in diffuse_textureN)
			std::stringstream ss;
			std::string number;
			std::string name = this->m_textures[i].type;
			if (name == "diffuse")
				ss << diffuseNr++; // Transfer GLuint to stream
			else if (name == "specular")
				ss << specularNr++; // Transfer GLuint to stream
			else if (name == "normal")
				ss << normalNr++; // Transfer GLuint to stream
			number = ss.str();
			std::string sss = ("material." + name + number).c_str();

			// Now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(program, ("material." + name + number).c_str()), i);
			// And finally bind the texture
			glBindTexture(GL_TEXTURE_2D, this->m_textures[i].id);
		}
		//enable or disable diffuseMapping depending if it's available
		if (diffuseNr == 1)
			glUniform1i(glGetUniformLocation(program, "diffuseMapping"), false);
		else
			glUniform1i(glGetUniformLocation(program, "diffuseMapping"), true);

		//enable or disable normalMapping depending if it's available
		if (normalNr == 1)
			glUniform1i(glGetUniformLocation(program, "normalMapping"), false);
		else
			glUniform1i(glGetUniformLocation(program, "normalMapping"), true);

		//enable or disable specularMapping depending if it's available
		if (specularNr == 1)
			glUniform1i(glGetUniformLocation(program, "specularMapping"), false);
		else
			glUniform1i(glGetUniformLocation(program, "specularMapping"), true);

		// Draw mesh
		glBindVertexArray(this->m_VAO);
		glDrawElements(GL_TRIANGLES, this->m_indices.size(), GL_UNSIGNED_INT, (GLvoid*)0);
		glBindVertexArray(0);

		// Always good practice to set everything back to defaults once configured.
		for (GLuint i = 0; i < this->m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	GLuint GetVBO() { return m_VBO; };

private:
	GLuint m_VAO, m_VBO, m_EBO;
	void Setup(bool dynamicDrawing)
	{
		ComputeTangentsJoinedIdenticalVerts();

		//initalise the opengl buffers
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		//vertex
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		if (!dynamicDrawing)
		{
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
				&m_vertices[0], GL_STATIC_DRAW);
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
				&m_vertices[0], GL_DYNAMIC_DRAW);
		}

		//element
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
			&m_indices[0], GL_STATIC_DRAW);

		//vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)0);
		//vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, Normal));
		//vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, TexCoords));
		//vertex colour
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, VertexColour));
		//vertex tangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, Tangent));
		//vertex bitangent
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*)offsetof(Vertex, BiTangent));

		glBindVertexArray(0);
		
	};

	void ComputeTangents()
	{
		//computes tangent and bi-tangents of the mesh's triangles
		//used for normal mapping
		for (int i = 0; i < m_vertices.size(); i += 3)
		{
			glm::vec3 &v0 = m_vertices[i].Position;
			glm::vec3 &v1 = m_vertices[i+1].Position;
			glm::vec3 &v2 = m_vertices[i+2].Position;

			glm::vec2 &uv0 = m_vertices[i].TexCoords;
			glm::vec2 &uv1 = m_vertices[i + 1].TexCoords;
			glm::vec2 &uv2 = m_vertices[i + 2].TexCoords;

			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			m_vertices[i].Tangent = tangent;
			m_vertices[i+1].Tangent = tangent;
			m_vertices[i+2].Tangent = tangent;

			m_vertices[i].BiTangent = bitangent;
			m_vertices[i + 1].BiTangent = bitangent;
			m_vertices[i + 2].BiTangent = bitangent;
		}
	}

	void ComputeTangentsJoinedIdenticalVerts()
	{
		for (int i = 0; i < m_indices.size(); i += 3)
		{
			glm::vec3 &v0 = m_vertices[m_indices[i]].Position;
			glm::vec3 &v1 = m_vertices[m_indices[i + 1]].Position;
			glm::vec3 &v2 = m_vertices[m_indices[i + 2]].Position;

			glm::vec2 &uv0 = m_vertices[m_indices[i]].TexCoords;
			glm::vec2 &uv1 = m_vertices[m_indices[i + 1]].TexCoords;
			glm::vec2 &uv2 = m_vertices[m_indices[i + 2]].TexCoords;

			glm::vec3 deltaPos1 = v1 - v0;
			glm::vec3 deltaPos2 = v2 - v0;

			glm::vec2 deltaUV1 = uv1 - uv0;
			glm::vec2 deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			m_vertices[m_indices[i]].Tangent = tangent;
			m_vertices[m_indices[i + 1]].Tangent = tangent;
			m_vertices[m_indices[i + 2]].Tangent = tangent;

			m_vertices[m_indices[i]].BiTangent = bitangent;
			m_vertices[m_indices[i + 1]].BiTangent = bitangent;
			m_vertices[m_indices[i + 2]].BiTangent = bitangent;
		}
	}
};


#endif
