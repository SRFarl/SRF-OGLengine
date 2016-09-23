#include "EntityEngine.h"
#include "GameObject.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, std::string meshName, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName, initPos, initRot, false), m_gameEE(gameEE)
	{
		m_rComp = new RenderComponent(_program, viewMat, projMat);

		m_renderNode = new RenderNode(&m_modelMatrix, m_rComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(&m_position, m_mComp);
		gameEE->AddMovableNode(m_movableNode);

		if (meshName.size() != 0)
			LoadModel(meshName);
	}

	~SpinningSquare()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveMovableNode(m_movableNode);
		delete m_movableNode;
		delete m_mComp;
	}

	void LoadModel(std::string meshName)
	{
		std::vector<glm::vec4> modelVertices;
		std::vector<glm::vec2> modelUVs;
		std::vector<glm::vec3> modelNormals;
		unsigned int numOfVertexes;

		if (obj::Load_OBJ(meshName.c_str(), modelVertices, modelUVs, modelNormals, numOfVertexes) == false)
		{
			std::cerr << "ERROR LOADING FILE" << std::endl;
		}

		m_rComp->m_unVertexCount = modelVertices.size();

		// create and bind a VAO to hold state for this model
		glGenVertexArrays(1, &m_rComp->m_glVertArray);
		glBindVertexArray(m_rComp->m_glVertArray);

		// Populate a vertex buffer
		glGenBuffers(1, &m_rComp->m_glVertBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_rComp->m_glVertBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * modelVertices.size(), &modelVertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// Populate a normal buffer
		glGenBuffers(1, &m_rComp->m_glNormBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_rComp->m_glNormBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * modelNormals.size(), &modelNormals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		// Populate a uv buffer
		glGenBuffers(1, &m_rComp->m_glUVBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_rComp->m_glUVBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * modelUVs.size(), &modelUVs[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		//textures
		int width, height;
		unsigned char* image;
		

		m_rComp->m_glMaterial.shininess = 32.0f;
		//diffuse
		glGenTextures(1, &m_rComp->m_glMaterial.diffuse);
		image = SOIL_load_image("Models/c_text.png", &width, &height, 0, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, m_rComp->m_glMaterial.diffuse);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		//specular
		glGenTextures(1, &m_rComp->m_glMaterial.specular);
		image = SOIL_load_image("Models/c_text_spec.png", &width, &height, 0, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, m_rComp->m_glMaterial.specular);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void Update(float deltaT)
	{
		//m_rotAngles.x += glm::radians(1.f) * deltaT;
		//m_rotAngles.y += glm::radians(1.f) * deltaT;
		//m_rotAngles.z += glm::radians(1.f) * deltaT;

		if (!m_isStatic)
		{
			UpdateObjectRotation();
			m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(m_quat);
		}
	}

private:
	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;
	EntityEngine* m_gameEE;

};