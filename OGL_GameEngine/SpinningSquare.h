#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName, initPos, initRot, false), m_gameEE(gameEE)
	{
		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;

		m_renderNode = new RenderNode(&m_modelMatrix, m_rComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(&m_position, m_mComp);
		gameEE->AddMovableNode(m_movableNode);

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