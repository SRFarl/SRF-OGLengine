#ifndef _ENTITY_COMPONENT_H_
#define _ENTITY_COMPONENT_H_
#include "Headers.h"
#include "Model.h"

//components are variables containers
//no logic occurs in a component

class RenderComponent
{
public:
	RenderComponent(GLuint _program, glm::mat4* viewMat, glm::mat4* projMat) : 
		m_viewMatrix(viewMat), 
		m_projMatrix(projMat)
	{
		m_program = _program;
		m_shaderModelMatLocation = glGetUniformLocation(_program, "modelMat");
		m_shaderViewMatLocation = glGetUniformLocation(_program, "viewMat");
		m_shaderProjMatLocation = glGetUniformLocation(_program, "projMat");
	}

	~RenderComponent()
	{
	}

	GLuint m_program;
	GLint m_shaderModelMatLocation, m_shaderViewMatLocation, m_shaderProjMatLocation;
	glm::mat4* m_viewMatrix, * m_projMatrix;
	Model* m_rcModel;
};

class MovableComponent
{
public:
	MovableComponent() :
		m_acceleration(0.0f),
		m_velocity(0.0f),
		m_skipThisFrame(false),
		m_mass(1.0f)
	{}

	float m_mass;
	bool m_skipThisFrame;
	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
};

class TransformComponent
{
public:
	TransformComponent(glm::vec3 _initPos, glm::vec3 _initRot, bool _isStatic) :
		m_position(_initPos),
		m_rotAngles(_initRot),
		m_quatRight(1.0f, 0.f, 0.f),
		m_quatUp(0.f, 1.0f, 0.f),
		m_quatForward(0.f, 0.f, 1.0f),
		m_isStatic(_isStatic)
	{}
	glm::vec3 m_position;

	glm::mat4 m_modelMatrix;

	glm::quat m_quat;
	glm::vec3 m_rotAngles;
	glm::vec3 m_quatRight;
	glm::vec3 m_quatUp;
	glm::vec3 m_quatForward;

	bool m_isStatic;
};

class SelectedComponent
{
public:
	SelectedComponent() :
		m_selected(false)
	{}
	bool m_selected;
};

class SphereCollsionComponent
{
public:
	SphereCollsionComponent(glm::vec3 _centrePoint, float _radius, float _damping) :
		m_centrePointOffset(_centrePoint),
		m_radius(_radius),
		m_damping(_damping)
	{}

	float m_damping;
	glm::vec3 m_centrePointOffset;
	float m_radius;

};

class AABBCollisionComponent
{
public:
	AABBCollisionComponent(glm::vec3 _min, glm::vec3 _max, glm::vec3 _mid, float _damping) :
		m_minOffset(_min),
		m_maxOffset(_max),
		m_midOffset(_mid),
		m_damping(_damping)
	{}

	float m_damping;
	glm::vec3 m_minOffset;
	glm::vec3 m_maxOffset;
	glm::vec3 m_midOffset;
};

class FlexBaseComponent
{
public:
	FlexBaseComponent(int numberOfParticles, int _numsubsteps) : m_flex(NULL), m_numSubsteps(_numsubsteps)
	{
		m_flex = flexCreateSolver(numberOfParticles, numberOfParticles, 96);
	};

	~FlexBaseComponent()
	{
		flexDestroySolver(m_flex);
	}

	FlexSolver* m_flex;
	int m_numSubsteps;
};

class FlexRigidComponent
{
public:
	FlexRigidComponent()
	{
	};

	FlexParams m_params;
	std::vector<Instance> m_instances;

	std::vector<Vec4> m_positions;
	std::vector<Vec4> m_restPositions;
	std::vector<Vec3> m_velocities;
	std::vector<Vec4> m_normals;
	std::vector<int> m_phases;
	std::vector<int> m_activeIndices;

	std::vector<int> m_rigidOffsets;
	std::vector<int> m_rigidIndices;
	std::vector<float> m_rigidCoefficients;
	std::vector<Quat> m_rigidRotations;
	std::vector<Vec3> m_rigidTranslations;
	std::vector<Vec3> m_rigidLocalPositions;
	std::vector<Vec4> m_rigidLocalNormals;

	std::vector<int> m_springIndices;
	std::vector<float> m_springLengths;
	std::vector<float> m_springStiffness;

	Vec3 m_scale;
	Vec3 m_offset;
	float m_radius;
	float m_clusterSpacing;
	float m_clusterRadius;
	float m_clusterStiffness;
	float m_linkRadius;
	float m_linkStiffness;
	float m_surfaceSampling;
	float m_volumeSampling;
	float m_skinningFalloff;
	float m_skinningMaxDistance;

};

class FlexClothComponent
{
public:
	FlexClothComponent()
	{
	};

	FlexParams m_params;
	Vec3 m_offset;
	float m_windTime;
	int m_phase;

	std::vector<Vec4> m_positions;
	std::vector<Vec4> m_restPositions;
	std::vector<Vec3> m_velocities;
	std::vector<Vec4> m_normals;
	std::vector<int> m_phases;
	std::vector<int> m_activeIndices;

	std::vector<int> m_springIndices;
	std::vector<float> m_springLengths;
	std::vector<float> m_springStiffness;

	std::vector<int> m_triangles;
	std::vector<Vec3> m_triangleNormals;

	float m_stretchStiffness;
	float m_bendStiffness;
	float m_shearStiffness;
};

#endif