#ifndef _FLEXRIGIDSLAMPLANE_H_
#define _FLEXRIGIDSLAMPLANE_H_
#include "Headers.h"
#include "GameObject.h"
#include "EntityEngine.h"
#include "Model.h"
#include "flexutil.h"

class FlexRigidObject : public GameObject
{
public:
	FlexRigidObject(const std::string meshName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot);

	virtual void Update(float deltaT) override;

	void UpdateModel();

private: //Entity
	FlexRigidComponent* m_frComp;
	FlexBaseComponent* m_fbComp;
	FlexRigidNode* m_frNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;
};

#endif