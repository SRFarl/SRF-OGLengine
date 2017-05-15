#include "FlexRigidObject.h"

FlexRigidObject::FlexRigidObject(const std::string meshName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, std::shared_ptr<EntityEngine> gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(meshName)
{
	//entity
	//transform
	m_transformComp = std::make_shared<TransformComponent>(initPos, initRot, false);
	m_transformNode = std::make_shared<TransformNode>(m_transformComp);
	gameEE->AddTransformNode(m_transformNode);

	//render
	m_rComp = std::make_shared<RenderComponent>(_program, viewMat, projMat);
	m_rComp->m_rcModel = modelAsset;
	m_renderNode = std::make_shared<RenderNode>(m_rComp, m_transformComp);
	gameEE->AddRenderNode(m_renderNode);

	//flex
	m_frComp = std::make_shared<FlexRigidComponent>();

	//init flex variables and params
	m_frComp->m_scale = Vec3(5.0f, 5.0f, 5.0f);
	m_frComp->m_offset = Vec3(initPos.x, initPos.y, initPos.z);
	m_frComp->m_clusterSpacing = 2.0f;
	m_frComp->m_clusterRadius = 2.0f;
	m_frComp->m_clusterStiffness = 0.55f;
	m_frComp->m_linkRadius = 0.0f;
	m_frComp->m_linkStiffness = 1.0f;
	m_frComp->m_surfaceSampling = 0.0f;
	m_frComp->m_volumeSampling = 4.0f;
	m_frComp->m_skinningFalloff = 2.0f;
	m_frComp->m_skinningMaxDistance = 100.0f;
	m_frComp->m_rigidOffsets.push_back(0);
	m_frComp->m_radius = 0.40f;

	flexutil::CreateSoftBody(modelAsset, m_frComp.get());

	m_frComp->m_radius *= 1.5f;
	m_frComp->m_normals.resize(m_frComp->m_positions.size());

	//create base component
	m_fbComp = std::make_shared<FlexBaseComponent>(m_frComp->m_positions.size(), 2);

	//set some flex params
	m_frComp->m_params.mGravity[0] = 0.0f;
	m_frComp->m_params.mGravity[1] = -9.8f;
	m_frComp->m_params.mGravity[2] = 0.0f;

	m_frComp->m_params.mWind[0] = 0.0f;
	m_frComp->m_params.mWind[1] = 0.0f;
	m_frComp->m_params.mWind[2] = 0.0f;

	m_frComp->m_params.mNumIterations = 4;
	m_frComp->m_params.mFluidRestDistance = 0.0f;
	m_frComp->m_params.mSolidRestDistance = m_frComp->m_radius;
	m_frComp->m_params.mAnisotropyScale = 1.0f;
	m_frComp->m_params.mAnisotropyMin = 0.1f;
	m_frComp->m_params.mAnisotropyMax = 2.0f;
	m_frComp->m_params.mSmoothing = 1.0f;
	m_frComp->m_params.mSolidPressure = 1.0f;

	m_frComp->m_params.mRadius = m_frComp->m_radius;
	m_frComp->m_params.mDynamicFriction = 0.35f;
	m_frComp->m_params.mStaticFriction = 0.0f;
	m_frComp->m_params.mParticleFriction = 0.25f;
	m_frComp->m_params.mInertiaBias = 0.001f;

	m_frComp->m_params.mDissipation = 0.0f;
	m_frComp->m_params.mShockPropagation = 0.0f;
	m_frComp->m_params.mDamping = 0.0f;
	m_frComp->m_params.mRestitution = 0.0f;
	m_frComp->m_params.mAdhesion = 0.0f;
	m_frComp->m_params.mSleepThreshold = 0.0f;
	m_frComp->m_params.mMaxSpeed = FLT_MAX;
	m_frComp->m_params.mViscosity = 0.0f;
	m_frComp->m_params.mVorticityConfinement = 0.0f;
	m_frComp->m_params.mLift = 0.0f;
	m_frComp->m_params.mCohesion = 0.025f;
	m_frComp->m_params.mSurfaceTension = 0.0f;
	m_frComp->m_params.mFreeSurfaceDrag = 0.0f;
	m_frComp->m_params.mBuoyancy = 1.0f;
	m_frComp->m_params.mPlasticThreshold = 0.0f;
	m_frComp->m_params.mPlasticCreep = 0.0f;

	m_frComp->m_params.mDrag = 0.0f;
	m_frComp->m_params.mCollisionDistance = m_frComp->m_params.mRadius * 0.5f;
	m_frComp->m_params.mParticleCollisionMargin = 0.0f;
	m_frComp->m_params.mShapeCollisionMargin = m_frComp->m_params.mRadius*0.25f;
	m_frComp->m_params.mRelaxationFactor = 1.3f;
	m_frComp->m_params.mDiffuseThreshold = 100.f;
	m_frComp->m_params.mDiffuseBuoyancy = 1.0f;
	m_frComp->m_params.mDiffuseDrag = 0.8f;
	m_frComp->m_params.mDiffuseBallistic = 16;
	m_frComp->m_params.mDiffuseSortAxis[0] = 0.0f;
	m_frComp->m_params.mDiffuseSortAxis[1] = 0.0f;
	m_frComp->m_params.mDiffuseSortAxis[2] = 0.0f;
	m_frComp->m_params.mDiffuseLifetime = 2.0f;

	(Vec4&)m_frComp->m_params.mPlanes[0] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	m_frComp->m_params.mNumPlanes = 1;
	m_frComp->m_params.mFluid = false;
	m_frComp->m_params.mRelaxationMode = eFlexRelaxationLocal;

	//create flex rigid node
	m_frNode = std::make_shared<FlexRigidNode>(m_rComp, m_fbComp, m_frComp);
	gameEE->AddFlexRigidNode(m_frNode);
}

void FlexRigidObject::Update(float deltaT)
{
	UpdateModel();
}

void FlexRigidObject::UpdateModel()
{
	//bind array object
	glBindVertexArray(m_rComp->m_program);

	//bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_rComp->m_rcModel->m_meshes[0].GetVBO());

	//input new buffer data)
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_rComp->m_rcModel->m_meshes[0].m_vertices.size(), &m_rComp->m_rcModel->m_meshes[0].m_vertices[0]);

	//unbind buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//unbind array object
	glBindVertexArray(0);
}
