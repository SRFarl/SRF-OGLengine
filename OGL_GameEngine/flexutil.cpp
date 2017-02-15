#include "flexutil.h"

void flexutil::CreateSoftBody(Model* modelAsset, FlexRigidComponent* flexBase)
{
	Instance instance;

	Mesh* mesh = new Mesh(SetModelToFlexMesh(modelAsset));

	mesh->Normalize();
	mesh->Transform(TranslationMatrix(Point3(flexBase->m_offset))*ScaleMatrix(flexBase->m_scale*flexBase->m_radius));

	instance.m_mesh = mesh;
	instance.m_color = Vec3(0.5f, 0.5f, 1.0f);
	instance.m_offset = flexBase->m_rigidTranslations.size();

	double createStart = GetSeconds();

	// create soft body definition
	FlexExtAsset* asset = flexExtCreateSoftFromMesh(
		(float*)&instance.m_mesh->m_positions[0],
		instance.m_mesh->m_positions.size(),
		(int*)&instance.m_mesh->m_indices[0],
		instance.m_mesh->m_indices.size(),
		flexBase->m_radius,
		flexBase->m_volumeSampling,
		flexBase->m_surfaceSampling,
		flexBase->m_clusterSpacing*flexBase->m_radius,
		flexBase->m_clusterRadius*flexBase->m_radius,
		flexBase->m_clusterStiffness,
		flexBase->m_linkRadius*flexBase->m_radius,
		flexBase->m_linkStiffness);

	double createEnd = GetSeconds();

	// create skinning
	const int maxWeights = 4;

	instance.m_skinningIndices.resize(instance.m_mesh->m_positions.size()*maxWeights);
	instance.m_skinningWeights.resize(instance.m_mesh->m_positions.size()*maxWeights);

	for (int i = 0; i < asset->mNumShapes; ++i)
		instance.m_rigidRestPoses.push_back(Vec3(&asset->mShapeCenters[i * 3]));

	double skinStart = GetSeconds();

	flexExtCreateSoftMeshSkinning(
		(float*)&instance.m_mesh->m_positions[0],
		instance.m_mesh->m_positions.size(),
		asset->mShapeCenters,
		asset->mNumShapes,
		flexBase->m_skinningFalloff,
		flexBase->m_skinningMaxDistance,
		&instance.m_skinningWeights[0],
		&instance.m_skinningIndices[0]);

	double skinEnd = GetSeconds();

	printf("Created soft in %f ms Skinned in %f\n", (createEnd - createStart)*1000.0f, (skinEnd - skinStart)*1000.0f);

	const int particleOffset = flexBase->m_positions.size();
	const int indexOffset = flexBase->m_rigidOffsets.back();

	// add particle data to solver
	for (int i = 0; i < asset->mNumParticles; ++i)
	{
		flexBase->m_positions.push_back(&asset->mParticles[i * 4]);
		flexBase->m_velocities.push_back(0.0f);

		const int phase = flexMakePhase(flexBase->m_instances.size(), eFlexPhaseSelfCollide | eFlexPhaseSelfCollideFilter);
		flexBase->m_phases.push_back(phase);
	}

	// add shape data to solver
	for (int i = 0; i < asset->mNumShapeIndices; ++i)
		flexBase->m_rigidIndices.push_back(asset->mShapeIndices[i] + particleOffset);

	for (int i = 0; i < asset->mNumShapes; ++i)
	{
		flexBase->m_rigidOffsets.push_back(asset->mShapeOffsets[i] + indexOffset);
		flexBase->m_rigidTranslations.push_back(Vec3(&asset->mShapeCenters[i * 3]));
		flexBase->m_rigidRotations.push_back(Quat());
		flexBase->m_rigidCoefficients.push_back(asset->mShapeCoefficients[i]);
	}

	// add link data to the solver 
	for (int i = 0; i < asset->mNumSprings; ++i)
	{
		flexBase->m_springIndices.push_back(asset->mSpringIndices[i * 2 + 0]);
		flexBase->m_springIndices.push_back(asset->mSpringIndices[i * 2 + 1]);

		flexBase->m_springStiffness.push_back(asset->mSpringCoefficients[i]);
		flexBase->m_springLengths.push_back(asset->mSpringRestLengths[i]);
	}

	flexExtDestroyAsset(asset);

	flexBase->m_instances.push_back(instance);
}

Mesh flexutil::SetModelToFlexMesh(Model *modelAsset)
{
	Mesh tempmesh;

	//vertices
	tempmesh.m_positions.resize(modelAsset->m_meshes[0].m_vertices.size());
	for (int i = 0; i < modelAsset->m_meshes[0].m_vertices.size(); i++)
		tempmesh.m_positions[i] = Point3(modelAsset->m_meshes[0].m_vertices[i].Position.x, modelAsset->m_meshes[0].m_vertices[i].Position.y, modelAsset->m_meshes[0].m_vertices[i].Position.z);

	//normal
	tempmesh.m_normals.resize(modelAsset->m_meshes[0].m_vertices.size());
	for (int i = 0; i < modelAsset->m_meshes[0].m_vertices.size(); i++)
		tempmesh.m_normals[i] = Vec3(modelAsset->m_meshes[0].m_vertices[i].Normal.x, modelAsset->m_meshes[0].m_vertices[i].Normal.y, modelAsset->m_meshes[0].m_vertices[i].Normal.z);

	//UV
	tempmesh.m_texcoords->resize(modelAsset->m_meshes[0].m_vertices.size());
	for (int i = 0; i < modelAsset->m_meshes[0].m_vertices.size(); i++)
		tempmesh.m_texcoords[0][i] = Vec2(modelAsset->m_meshes[0].m_vertices[i].TexCoords.x, modelAsset->m_meshes[0].m_vertices[i].Normal.y);

	//Indices
	tempmesh.m_indices.resize(modelAsset->m_meshes[0].m_indices.size());
	for (int i = 0; i < modelAsset->m_meshes[0].m_indices.size(); i++)
		tempmesh.m_indices[i] = modelAsset->m_meshes[0].m_indices[i];

	return tempmesh;
}

void flexutil::CalculateRigidOffsets(const Vec4* restPositions, const int* offsets, const int* indices, int numRigids, Vec3* localPositions)
{
	int count = 0;

	for (int r = 0; r < numRigids; ++r)
	{
		const int startIndex = offsets[r];
		const int endIndex = offsets[r + 1];

		const int n = endIndex - startIndex;

		assert(n);

		Vec3 com;

		for (int i = startIndex; i < endIndex; ++i)
		{
			const int r = indices[i];

			com += Vec3(restPositions[r]);
		}

		com /= float(n);

		for (int i = startIndex; i < endIndex; ++i)
		{
			const int r = indices[i];

			localPositions[count++] = Vec3(restPositions[r]) - com;
		}
	}
}

void flexutil::FindSoftBodyOffsets(FlexRigidComponent* flexRigid, RenderComponent* renderComp)
{
	for (int s = 0; s < int(flexRigid->m_instances.size()); ++s)
	{
		const Instance& instance = flexRigid->m_instances[s];

		Mesh m;
		m.m_positions.resize(instance.m_mesh->m_positions.size());
		m.m_normals.resize(instance.m_mesh->m_normals.size());
		m.m_indices = instance.m_mesh->m_indices;

		for (int i = 0; i < int(instance.m_mesh->m_positions.size()); ++i)
		{
			Vec3 softPos;
			Vec3 softNormal;

			for (int w = 0; w < 4; ++w)
			{
				const int cluster = instance.m_skinningIndices[i * 4 + w];
				const float weight = instance.m_skinningWeights[i * 4 + w];

				if (cluster > -1)
				{
					// offset in the global constraint array
					int rigidIndex = cluster + instance.m_offset;

					Vec3 localPos = Vec3(instance.m_mesh->m_positions[i]) - instance.m_rigidRestPoses[cluster];

					Vec3 skinnedPos = flexRigid->m_rigidTranslations[rigidIndex] + Rotate(flexRigid->m_rigidRotations[rigidIndex], localPos);
					Vec3 skinnedNormal = Rotate(flexRigid->m_rigidRotations[rigidIndex], instance.m_mesh->m_normals[i]);

					softPos += skinnedPos*weight;
					softNormal += skinnedNormal*weight;
				}
			}

			m.m_positions[i] = Point3(softPos);
			m.m_normals[i] = softNormal;
		}

		for (int j = 0; j < m.m_indices.size(); j++)
		{
			renderComp->m_rcModel->m_meshes[0].m_vertices[renderComp->m_rcModel->m_meshes[0].m_indices[j]].Position = glm::vec3(m.m_positions[m.m_indices[j]].x, m.m_positions[m.m_indices[j]].y, m.m_positions[m.m_indices[j]].z);
			renderComp->m_rcModel->m_meshes[0].m_vertices[renderComp->m_rcModel->m_meshes[0].m_indices[j]].Normal = glm::vec3(m.m_normals[m.m_indices[j]].x, m.m_normals[m.m_indices[j]].y, m.m_normals[m.m_indices[j]].z);
		}

		//apply new data
		//for (int j = 0; j < m.m_positions.size(); j++)
		//m_rComp->m_rcModel->m_meshes[0].m_vertices[j].Position = glm::vec3(m.m_positions[j].x, m.m_positions[j].y, m.m_positions[j].z);

	}
}

void flexutil::MakeBaseStaticInCorners(FlexRigidComponent* flexRigid)
{
	//find z width and x width
	float zmax = flexRigid->m_positions[0].z;
	float zmin = flexRigid->m_positions[0].z;
	float xmax = flexRigid->m_positions[0].x;
	float xmin = flexRigid->m_positions[0].x;

	for (int i = 0; i < flexRigid->m_positions.size(); i++)
	{
		if (flexRigid->m_positions[i].z > zmax)
		{
			zmax = flexRigid->m_positions[i].z;
		}

		if (flexRigid->m_positions[i].z < zmin)
		{
			zmin = flexRigid->m_positions[i].z;
		}

		if (flexRigid->m_positions[i].x > xmax)
		{
			xmax = flexRigid->m_positions[i].x;
		}

		if (flexRigid->m_positions[i].x < xmin)
		{
			xmin = flexRigid->m_positions[i].x;
		}
	}

	float zwidth = zmax - zmin;
	float xwidth = xmax - xmin;
	float zmaxthreshold = zmax - (zwidth * 0.1f);
	float zminthreshold = zmin + (zwidth * 0.1f);
	float xmaxthreshold = xmax - (xwidth * 0.1f);
	float xminthreshold = xmin + (xwidth * 0.1f);

	//filter out positions in the corners
	std::vector<int> listOfCornerPositions;
	for (int i = 0; i < flexRigid->m_positions.size(); i++)
	{
		if (flexRigid->m_positions[i].z > zmaxthreshold)
			listOfCornerPositions.push_back(i);

		if (flexRigid->m_positions[i].z < zminthreshold)
			listOfCornerPositions.push_back(i);
	}

	for (int i = 0; i < listOfCornerPositions.size(); i++)
	{
		if (!(flexRigid->m_positions[listOfCornerPositions[i]].x > xmaxthreshold))
		{
			if (!(flexRigid->m_positions[listOfCornerPositions[i]].x < xminthreshold))
			{
				listOfCornerPositions.erase(listOfCornerPositions.begin() + i);
				i--;
			}
		}
	}

	std::vector<int> corner1;
	std::vector<int> corner2;
	std::vector<int> corner3;
	std::vector<int> corner4;

	for (int i = 0; i < listOfCornerPositions.size(); i++)
	{
		if (flexRigid->m_positions[listOfCornerPositions[i]].z > zmaxthreshold &&
			flexRigid->m_positions[listOfCornerPositions[i]].x > xmaxthreshold)
			corner1.push_back(listOfCornerPositions[i]);

		if (flexRigid->m_positions[listOfCornerPositions[i]].z > zmaxthreshold &&
			flexRigid->m_positions[listOfCornerPositions[i]].x < xminthreshold)
			corner2.push_back(listOfCornerPositions[i]);

		if (flexRigid->m_positions[listOfCornerPositions[i]].z < zminthreshold &&
			flexRigid->m_positions[listOfCornerPositions[i]].x > xmaxthreshold)
			corner3.push_back(listOfCornerPositions[i]);

		if (flexRigid->m_positions[listOfCornerPositions[i]].z < zminthreshold &&
			flexRigid->m_positions[listOfCornerPositions[i]].x < xminthreshold)
			corner4.push_back(listOfCornerPositions[i]);
	}

	//set the lowest position in corner 1 to static
	int smallestYCorner1 = 0;
	for (int i = 0; i < corner1.size(); i++)
	{
		if (flexRigid->m_positions[corner1[i]].y < flexRigid->m_positions[corner1[smallestYCorner1]].y)
			smallestYCorner1 = i;
	}
	flexRigid->m_positions[corner1[smallestYCorner1]].w = 0.0f;

	//set the lowest position in corner 2 to static
	int smallestYCorner2 = 0;
	for (int i = 0; i < corner2.size(); i++)
	{
		if (flexRigid->m_positions[corner2[i]].y < flexRigid->m_positions[corner2[smallestYCorner2]].y)
			smallestYCorner2 = i;
	}
	flexRigid->m_positions[corner2[smallestYCorner2]].w = 0.0f;

	//set the lowest position in corner 3 to static
	int smallestYCorner3 = 0;
	for (int i = 0; i < corner3.size(); i++)
	{
		if (flexRigid->m_positions[corner3[i]].y < flexRigid->m_positions[corner3[smallestYCorner3]].y)
			smallestYCorner3 = i;
	}
	flexRigid->m_positions[corner3[smallestYCorner3]].w = 0.0f;

	//set the lowest position in corner 4 to static
	int smallestYCorner4 = 0;
	for (int i = 0; i < corner4.size(); i++)
	{
		if (flexRigid->m_positions[corner4[i]].y < flexRigid->m_positions[corner4[smallestYCorner4]].y)
			smallestYCorner4 = i;
	}
	flexRigid->m_positions[corner4[smallestYCorner4]].w = 0.0f;

}

void flexutil::ErrorCallback(FlexErrorSeverity, const char* msg, const char* file, int line)
{
	printf("Flex: %s - %s:%d\n", msg, file, line);
	assert(0);
}
