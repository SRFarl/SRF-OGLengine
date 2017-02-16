#ifndef _FLEXCLOTHOBJECT_H_
#define _FLEXCLOTHOBJECT_H_
#include "Headers.h"
#include "GameObject.h"
#include "EntityEngine.h"
#include "Model.h"
#include "flexutil.h"

class FlexClothObject : public GameObject
{
public:
	FlexClothObject(const std::string meshName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot);

	virtual void Update(float deltaT) override;

	void UpdateModel();

private: //Entity
	FlexClothComponent* m_fcComp;
	FlexBaseComponent* m_fbComp;
	FlexClothNode* m_fcNode;

	RenderComponent* m_rComp;
	RenderNode* m_renderNode;

	TransformComponent* m_transformComp;
	TransformNode* m_transformNode;

	EntityEngine* m_gameEE;
};

#endif