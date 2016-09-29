#ifndef _MODEL_H_
#define _MODEL_H_
#include "Headers.h"
#include "Mesh.h"

GLint TextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

class Model
{
public:
	Model(GLchar* path, std::string _modelName) : m_name(_modelName)
	{
		LoadModel(path);
	}

	~Model()
	{
		m_meshes.clear();
	}

	std::string GetName(){ return m_name; }

	std::vector<Mesh> m_meshes;

private:
	std::string m_directory;
	std::string m_name;

	void LoadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		m_directory = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	};
	void ProcessNode(aiNode* node, const aiScene* scene)
	{
		// Process all the node's meshes (if any)
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_meshes.push_back(this->ProcessMesh(mesh, scene));
		}
		// Then do the same for each of its children
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	};

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			// Process vertex positions, normals and texture coordinates
			glm::vec3 _vec3;
			_vec3.x = mesh->mVertices[i].x;
			_vec3.y = mesh->mVertices[i].y;
			_vec3.z = mesh->mVertices[i].z;
			vertex.Position = _vec3;

			_vec3.x = mesh->mNormals[i].x;
			_vec3.y = mesh->mNormals[i].y;
			_vec3.z = mesh->mNormals[i].z;
			vertex.Normal = _vec3;

			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 _vec2;
				_vec2.x = mesh->mTextureCoords[0][i].x;
				_vec2.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = _vec2;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		// Process indices
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = LoadMaterialTextures(material,
				aiTextureType_DIFFUSE, "diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = LoadMaterialTextures(material,
				aiTextureType_SPECULAR, "specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = LoadMaterialTextures(material,
				aiTextureType_HEIGHT, "normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

		return Mesh(&vertices, &indices, &textures);
	};
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->m_directory);
			texture.type = typeName;
			textures.push_back(texture);
		}
		return textures;
	};
};

#endif