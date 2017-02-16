#include "FlexClothObject.h"

FlexClothObject::FlexClothObject(const std::string meshName, Model* modelAsset, GLuint _program, glm::mat4* viewMat, glm::mat4* projMat, EntityEngine* gameEE, glm::vec3 initPos, glm::vec3 initRot) : GameObject(meshName), m_gameEE(gameEE)
{
	//entity
	//transform
	m_transformComp = new TransformComponent(initPos, initRot, false);
	m_transformNode = new TransformNode(m_transformComp);
	gameEE->AddTransformNode(m_transformNode);

	//render
	m_rComp = new RenderComponent(_program, viewMat, projMat);
	m_rComp->m_rcModel = modelAsset;
	m_renderNode = new RenderNode(m_rComp, m_transformComp);
	gameEE->AddRenderNode(m_renderNode);

	//flex
	m_fcComp = new FlexClothComponent();

	m_fcComp->m_windTime = 0.0f;
	m_fcComp->m_offset = Vec3(initPos.x, initPos.y, initPos.z);

	m_fcComp->m_params.mGravity[0] = 0.0f;
	m_fcComp->m_params.mGravity[2] = -9.8f;
	m_fcComp->m_params.mGravity[1] = 0.0f;

	m_fcComp->m_params.mWind[0] = 0.0f;
	m_fcComp->m_params.mWind[1] = 0.0f;
	m_fcComp->m_params.mWind[2] = 0.0f;

	m_fcComp->m_params.mNumIterations = 8;
	m_fcComp->m_params.mFluidRestDistance = 0.0f;
	m_fcComp->m_params.mSolidRestDistance = 0.05f;
	m_fcComp->m_params.mAnisotropyScale = 1.0f;
	m_fcComp->m_params.mAnisotropyMin = 0.1f;
	m_fcComp->m_params.mAnisotropyMax = 2.0f;
	m_fcComp->m_params.mSmoothing = 1.0f;
	m_fcComp->m_params.mSolidPressure = 1.0f;

	m_fcComp->m_params.mRadius = 0.0015f;
	m_fcComp->m_params.mDynamicFriction = 0.25f;
	m_fcComp->m_params.mStaticFriction = 0.0f;
	m_fcComp->m_params.mParticleFriction = m_fcComp->m_params.mDynamicFriction*0.1f;
	m_fcComp->m_params.mInertiaBias = 0.001f;

	m_fcComp->m_params.mDissipation = 0.0f;
	m_fcComp->m_params.mShockPropagation = 0.0f;
	m_fcComp->m_params.mDamping = 0.0f;
	m_fcComp->m_params.mRestitution = 0.0f;
	m_fcComp->m_params.mAdhesion = 0.0f;
	m_fcComp->m_params.mSleepThreshold = 0.0f;
	m_fcComp->m_params.mMaxSpeed = FLT_MAX;
	m_fcComp->m_params.mViscosity = 0.0f;
	m_fcComp->m_params.mVorticityConfinement = 0.0f;
	m_fcComp->m_params.mLift = 0.0f;
	m_fcComp->m_params.mCohesion = 0.025f;
	m_fcComp->m_params.mSurfaceTension = 0.0f;
	m_fcComp->m_params.mFreeSurfaceDrag = 0.0f;
	m_fcComp->m_params.mBuoyancy = 1.0f;
	m_fcComp->m_params.mPlasticThreshold = 0.0f;
	m_fcComp->m_params.mPlasticCreep = 0.0f;

	m_fcComp->m_params.mDrag = 0.05f;
	m_fcComp->m_params.mCollisionDistance = m_fcComp->m_params.mRadius;
	m_fcComp->m_params.mShapeCollisionMargin = m_fcComp->m_params.mRadius*0.1f;
	m_fcComp->m_params.mRelaxationFactor = 1.3f;
	m_fcComp->m_params.mDiffuseThreshold = 100.f;
	m_fcComp->m_params.mDiffuseBuoyancy = 1.0f;
	m_fcComp->m_params.mDiffuseDrag = 0.8f;
	m_fcComp->m_params.mDiffuseBallistic = 16;
	m_fcComp->m_params.mDiffuseSortAxis[0] = 0.0f;
	m_fcComp->m_params.mDiffuseSortAxis[1] = 0.0f;
	m_fcComp->m_params.mDiffuseSortAxis[2] = 0.0f;
	m_fcComp->m_params.mDiffuseLifetime = 2.0f;

	m_fcComp->m_params.mFluid = false;
	m_fcComp->m_params.mRelaxationMode = eFlexRelaxationLocal;

	m_fcComp->m_stretchStiffness = 1.0f;
	m_fcComp->m_bendStiffness = 0.8f;
	m_fcComp->m_shearStiffness = 0.5f;

	m_fcComp->m_phase = flexMakePhase(0, eFlexPhaseSelfCollide);

	flexutil::CreateSpringGrid(m_fcComp, &m_rComp->m_rcModel->m_meshes[0].m_vertices, &m_rComp->m_rcModel->m_meshes[0].m_indices, Vec3(0.0f, 0.0f, 0.0f));

	//create base component
	m_fbComp = new FlexBaseComponent(m_fcComp->m_positions.size(), 2);

	m_fcComp->m_positions[0].w = 0.0f;

	//create flex cloth node
	m_fcNode = new FlexClothNode(m_rComp, m_fbComp, m_fcComp);
	gameEE->AddFlexClothNode(m_fcNode);
}

void FlexClothObject::Update(float deltaT)
{
	//wind
	m_fcComp->m_windTime += deltaT;
	const Vec3 kWindDir = Vec3(3.0f, 15.0f, 0.0f);
	const float kNoise = fabsf(Perlin1D(m_fcComp->m_windTime*0.05f, 2, 0.25f));
	Vec3 wind = 1.0f*kWindDir*Vec3(kNoise, kNoise*0.1f, -kNoise*0.1f);

	m_fcComp->m_params.mWind[0] = wind.x;
	m_fcComp->m_params.mWind[1] = wind.y;
	m_fcComp->m_params.mWind[2] = wind.z;

	UpdateModel();
}

void FlexClothObject::UpdateModel()
{
	for (int j = 0; j < m_rComp->m_rcModel->m_meshes[0].m_indices.size(); j++)
	{
		m_rComp->m_rcModel->m_meshes[0].m_vertices[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].Position = glm::vec3(m_fcComp->m_positions[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].x, m_fcComp->m_positions[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].y, m_fcComp->m_positions[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].z);
		m_rComp->m_rcModel->m_meshes[0].m_vertices[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].Normal = glm::vec3(m_fcComp->m_normals[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].x, m_fcComp->m_normals[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].y, m_fcComp->m_normals[m_rComp->m_rcModel->m_meshes[0].m_indices[j]].z);
	}

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