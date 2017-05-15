#ifndef _FLEXRIGIDOBJECT_H_
#define _FLEXRIGIDOBJECT_H_
#include "Headers.h"
#include "GameObject.h"
#include "EntityEngine.h"
#include "Model.h"
#include "flexutil.h"

class FlexRigidObject : public GameObject
{
public:
	FlexRigidObject(const std::string meshName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, std::shared_ptr<EntityEngine> gameEE, glm::vec3 initPos, glm::vec3 initRot);

	virtual void Update(float deltaT) override;

	void UpdateModel();

private: //Entity
	std::shared_ptr<FlexRigidComponent> m_frComp;
	std::shared_ptr<FlexBaseComponent> m_fbComp;
	std::shared_ptr<FlexRigidNode> m_frNode;

	std::shared_ptr<RenderComponent> m_rComp;
	std::shared_ptr<RenderNode> m_renderNode;

	std::shared_ptr<TransformComponent> m_transformComp;
	std::shared_ptr<TransformNode> m_transformNode;

	std::shared_ptr<EntityEngine> m_gameEE;
};

#endif