#ifndef _MODEL_H_
#define _MODEL_H_
#include "Headers.h"
#include "ModelMesh.h"

GLint TextureFromFile(const char* path, std::string directory);

class Model
{
public:
	Model(GLchar* path, std::string _modelName);

	~Model();

	std::string GetName();

	void BuildAABB(glm::vec3 &min, glm::vec3 &max, glm::vec3 &mid);

	std::vector<ModelMesh> m_meshes;

private:
	std::string m_directory;
	std::string m_name;

	void LoadModel(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);

	ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif