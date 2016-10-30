#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class SpinningSquare : public GameObject
{
public:
	SpinningSquare(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(gameModelName), m_gameEE(gameEE)
	{
		m_transformComp = new TransformComponent(initPos, initRot);
		m_transformNode = new TransformNode(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(m_mComp, m_transformComp);
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

		m_gameEE->RemoveTransformNode(m_transformNode);
		delete m_transformNode;
		delete m_transformComp;
	}

	void Update(float deltaT)
	{
		m_transformComp->m_rotAngles.y += glm::radians(0.1f);

	}

private:
	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;

};