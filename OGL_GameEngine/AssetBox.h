#ifndef _ASSETBOX_H_
#define _ASSETBOX_H_
#include "Headers.h"
#include "OBJLoader_SRF.h"

class AssetBox
{
public:
	AssetBox(){};

	void LoadAsset(std::string _assetName, std::string meshName, std::string diffuseName, std::string specularName, float _shininess)
	{
		Asset temp;
		temp.assetName = _assetName;

		std::vector<glm::vec4> modelVertices;
		std::vector<glm::vec2> modelUVs;
		std::vector<glm::vec3> modelNormals;
		unsigned int numOfVertexes;

		if (obj::Load_OBJ(meshName.c_str(), modelVertices, modelUVs, modelNormals, numOfVertexes) == false)
		{
			std::cerr << "ERROR LOADING FILE" << std::endl;
		}

		temp.m_unVertexCount = modelVertices.size();

		// create and bind a VAO to hold state for this model
		glGenVertexArrays(1, &temp.m_glVertArray);
		glBindVertexArray(temp.m_glVertArray);

		// Populate a vertex buffer
		glGenBuffers(1, &temp.m_glVertBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.m_glVertBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * modelVertices.size(), &modelVertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// Populate a normal buffer
		glGenBuffers(1, &temp.m_glNormBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.m_glNormBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelNormals.size(), &modelNormals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// Populate a uv buffer
		glGenBuffers(1, &temp.m_glUVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, temp.m_glUVBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * modelUVs.size(), &modelUVs[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		//textures
		int width, height;
		unsigned char* image;

		temp.m_glMaterial.shininess = _shininess;

		//diffuse
		if (!diffuseName.empty())
		{
			glGenTextures(1, &temp.m_glMaterial.diffuse);
			image = SOIL_load_image(diffuseName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glBindTexture(GL_TEXTURE_2D, temp.m_glMaterial.diffuse);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);
			SOIL_free_image_data(image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			temp.m_glMaterial.diffuseSet = 1.0f;
		}
		else {
			temp.m_glMaterial.diffuseSet = 0.0f; 
		}

		//specular
		if (!specularName.empty())
		{
			glGenTextures(1, &temp.m_glMaterial.specular);
			image = SOIL_load_image(specularName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glBindTexture(GL_TEXTURE_2D, temp.m_glMaterial.specular);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);
			SOIL_free_image_data(image);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			temp.m_glMaterial.specularSet = 1.0f;
		}
		else {
			temp.m_glMaterial.specularSet = 0.0f;
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		assetList.push_back(temp);
	}

	void RemoveAsset(const std::string _assetName)
	{
		for (std::vector<Asset>::iterator it = assetList.begin(); it != assetList.end();)
		{
			if (it->assetName == _assetName)
			{
				assetList.erase(it);
				return;
			}

			it++;
		}
	}

	Asset* GetAsset(std::string _assetName)
	{
		for (std::vector<Asset>::iterator it = assetList.begin(); it != assetList.end();)
		{
			if (it->assetName == _assetName)
				return &(*it);

			it++;
		}
	}

	void BindAsset(const std::string _assetName)
	{
		for (std::vector<Asset>::iterator it = assetList.begin(); it != assetList.end();)
		{
			if (it->assetName == _assetName)
				glBindVertexArray(it->m_glVertArray);

			it++;
		}

		//safe bind as it failed
		glBindVertexArray(0);
	}

private:
	std::vector<Asset> assetList;

};

#endif