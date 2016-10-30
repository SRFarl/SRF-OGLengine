#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName, initPos, false), m_gameEE(gameEE)
	{
		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(&m_modelMatrix, m_rComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(&m_position, m_mComp);
		gameEE->AddMovableNode(m_movableNode);

		m_roComp = new RotationComponent(initRot);
		m_rotNode = new RotationNode(m_roComp);
		gameEE->AddRotationNode(m_rotNode);

	}

	~SpinningSquare()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveMovableNode(m_movableNode);
		delete m_movableNode;
		delete m_mComp;

		m_gameEE->RemoveRotationNode(m_rotNode);
		delete m_rotNode;
		delete m_roComp;
	}

	void Update(float deltaT)
	{

		m_roComp->m_rotAngles.y += glm::radians(0.1f);

		if (!m_isStatic)
		{
			m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(m_roComp->m_quat);
		}
	}

private:
	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	RotationComponent* m_roComp;
	RotationNode* m_rotNode;

	EntityEngine* m_gameEE;

};