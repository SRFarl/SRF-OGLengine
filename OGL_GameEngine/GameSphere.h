#include "EntityEngine.h"
#include "GameObject.h"
#include "Model.h"

class GameSphere : public GameObject
{
public:
	GameSphere(const std::string gameModelName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot, bool _gravity) : GameObject(gameModelName), m_gameEE(gameEE), gravity(_gravity)
	{
		m_transformComp = new TransformComponent(initPos, initRot, false);
		m_transformNode = new TransformNode(m_transformComp);
		gameEE->AddTransformNode(m_transformNode);

		m_rComp = new RenderComponent(_program, viewMat, projMat);
		m_rComp->m_rcModel = modelAsset;
		m_renderNode = new RenderNode(m_rComp, m_transformComp);
		gameEE->AddRenderNode(m_renderNode);

		m_mComp = new MovableComponent();
		m_movableNode = new MovableNode(m_mComp, m_transformComp);
		gameEE->AddMovableNode(m_movableNode);

		m_scComponent = new SphereCollsionComponent(glm::vec3(0.0f), 1.0f, 0.95f);
		m_scNode = new SphereCollisionNode(m_scComponent, m_transformComp, m_mComp);
		gameEE->AddSphereCollisionNode(m_scNode);
	}

	~GameSphere()
	{
		m_gameEE->RemoveRenderNode(m_renderNode);
		delete m_renderNode;
		delete m_rComp;

		m_gameEE->RemoveTransformNode(m_transformNode);
		delete m_transformNode;
		delete m_transformComp;

		m_gameEE->RemoveMovableNode(m_movableNode);
		delete m_mComp;
		delete m_movableNode;

		m_gameEE->RemoveSphereCollisionNode(m_scNode);
		delete m_scComponent;
		delete m_scNode;
	}

	void Update(float deltaT)
	{
		if (gravity)
			m_movableNode->movable->m_acceleration += glm::vec3(0, -9.8f, 0);
	}

private:
	bool gravity;

private:
	SphereCollsionComponent* m_scComponent;
	SphereCollisionNode* m_scNode;

	MovableComponent* m_mComp;
	MovableNode* m_movableNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;

};