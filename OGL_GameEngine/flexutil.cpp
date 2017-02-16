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

void flexutil::CreateSpringGrid(FlexClothComponent* flexCloth, std::vector<Vertex>* vertices, std::vector<GLuint>* indices, Vec3 velocity, float invMass /*= 1.0f*/)
{
	//loop over every vertice of the mesh
	for (int i = 0; i < vertices->size(); i++)
	{
		//create position, phase, velocity for each point
		flexCloth->m_positions.push_back(Vec4(flexCloth->m_offset.x + (*vertices)[i].Position.x, flexCloth->m_offset.y + (*vertices)[i].Position.y, flexCloth->m_offset.z + (*vertices)[i].Position.z, invMass));
		flexCloth->m_velocities.push_back(velocity);
		flexCloth->m_phases.push_back(flexCloth->m_phase);
	}

	//loop over every triangle in the mesh
	for (int i = 0; i < indices->size(); i += 3)
	{
		flexCloth->m_triangles.push_back((*indices)[i]);
		flexCloth->m_triangles.push_back((*indices)[i + 1]);
		flexCloth->m_triangles.push_back((*indices)[i + 2]);

		//create temp normal
		flexCloth->m_triangleNormals.push_back(Vec3(0.0f, 1.0f, 0.0f));
	}

	//create springs
	for (int i = 0; i < indices->size(); i += 3)
	{
		//create spring for the first edge of the triangle
		if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i], (*indices)[i + 1]))
			flexutil::CreateSpring(flexCloth, (*indices)[i], (*indices)[i + 1], flexCloth->m_stretchStiffness);

		//create spring for the second edge of triangle
		if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 1], (*indices)[i + 2]))
			flexutil::CreateSpring(flexCloth, (*indices)[i + 1], (*indices)[i + 2], flexCloth->m_stretchStiffness);

		//create spring for the third edge of triangle
		if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i], (*indices)[i + 2]))
			flexutil::CreateSpring(flexCloth, (*indices)[i], (*indices)[i + 2], flexCloth->m_stretchStiffness);

		//create spring for opposite vertices across two triangles sharing an edge
		for (int j = 0; j < indices->size(); j += 3)
		{
			//skip for same triangle
			if (j == i)
				continue;

			//check to see if v1 and v2 share any other triangle edges
			if ((*indices)[i] == (*indices)[j] && (*indices)[i + 1] == (*indices)[j + 1] ||
				(*indices)[i] == (*indices)[j + 1] && (*indices)[i + 1] == (*indices)[j])
			{
				//triangle edge shared, create some spring from t1v3 - t2v3
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 2], (*indices)[j + 2]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 2], (*indices)[j + 2], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i] == (*indices)[j + 1] && (*indices)[i + 1] == (*indices)[j + 2] ||
				(*indices)[i] == (*indices)[j + 2] && (*indices)[i + 1] == (*indices)[j + 1])
			{
				//triangle edge shared, create some spring t1v3 - t2v1
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 2], (*indices)[j]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 2], (*indices)[j], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i] == (*indices)[j] && (*indices)[i + 1] == (*indices)[j + 2] ||
				(*indices)[i] == (*indices)[j + 2] && (*indices)[i + 1] == (*indices)[j])
			{
				//triangle edge shared, create some spring t1v3 - t2v2
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 2], (*indices)[j + 1]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 2], (*indices)[j + 1], flexCloth->m_stretchStiffness);
				continue;
			}

			//check to see if v2 and v3 share any other triangle edges
			if ((*indices)[i + 1] == (*indices)[j] && (*indices)[i + 2] == (*indices)[j + 1] ||
				(*indices)[i + 1] == (*indices)[j + 1] && (*indices)[i + 2] == (*indices)[j])
			{
				//triangle edge shared, create some spring t1v1 - t2v3
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i], (*indices)[j + 2]))
					flexutil::CreateSpring(flexCloth, (*indices)[i], (*indices)[j + 2], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i + 1] == (*indices)[j + 1] && (*indices)[i + 2] == (*indices)[j + 2] ||
				(*indices)[i + 1] == (*indices)[j + 2] && (*indices)[i + 2] == (*indices)[j + 1])
			{
				//triangle edge shared, create some spring t1v1 - t2v1
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i], (*indices)[j]))
					flexutil::CreateSpring(flexCloth, (*indices)[i], (*indices)[j], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i + 1] == (*indices)[j] && (*indices)[i + 2] == (*indices)[j + 2] ||
				(*indices)[i + 1] == (*indices)[j + 2] && (*indices)[i + 2] == (*indices)[j])
			{
				//triangle edge shared, create some spring t1v1 - t2v2
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i], (*indices)[j + 1]))
					flexutil::CreateSpring(flexCloth, (*indices)[i], (*indices)[j + 1], flexCloth->m_stretchStiffness);
				continue;
			}

			//check to see if v1 and v3 share any other triangle edges
			if ((*indices)[i] == (*indices)[j] && (*indices)[i + 2] == (*indices)[j + 1] ||
				(*indices)[i] == (*indices)[j + 1] && (*indices)[i + 2] == (*indices)[j])
			{
				//triangle edge shared, create some spring t1v2 - t2v3
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 1], (*indices)[j + 2]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 1], (*indices)[j + 2], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i] == (*indices)[j + 1] && (*indices)[i + 2] == (*indices)[j + 2] ||
				(*indices)[i] == (*indices)[j + 2] && (*indices)[i + 2] == (*indices)[j + 1])
			{
				//triangle edge shared, create some spring t1v3 - t2v1
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 1], (*indices)[j]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 1], (*indices)[j], flexCloth->m_stretchStiffness);
				continue;
			}

			if ((*indices)[i] == (*indices)[j] && (*indices)[i + 2] == (*indices)[j + 2] ||
				(*indices)[i] == (*indices)[j + 2] && (*indices)[i + 2] == (*indices)[j])
			{
				//triangle edge shared, create some spring t1v3 - t2v2
				if (!flexutil::DoesSpringAlreadyExist(flexCloth, (*indices)[i + 1], (*indices)[j + 1]))
					flexutil::CreateSpring(flexCloth, (*indices)[i + 1], (*indices)[j + 1], flexCloth->m_stretchStiffness);
				continue;
			}
		}
	}
}

bool flexutil::DoesSpringAlreadyExist(FlexClothComponent* flexCloth, int startIndice, int endIndice)
{
	for (int i = 0; i < flexCloth->m_springIndices.size(); i += 2)
	{
		if (flexCloth->m_springIndices[i] == startIndice && flexCloth->m_springIndices[i + 1] == endIndice ||
			flexCloth->m_springIndices[i + 1] == startIndice && flexCloth->m_springIndices[i] == endIndice)
			return true;
	}

	return false;
}

void flexutil::CreateSpring(FlexClothComponent* flexCloth, int i, int j, float stiffness, float give /*= 0.0f*/)
{
	//taken from flex/demo/helpers.h
	flexCloth->m_springIndices.push_back(i);
	flexCloth->m_springIndices.push_back(j);
	flexCloth->m_springLengths.push_back((1.0f + give)*Length(Vec3(flexCloth->m_positions[i]) - Vec3(flexCloth->m_positions[j])));
	flexCloth->m_springStiffness.push_back(stiffness);
}

void flexutil::ErrorCallback(FlexErrorSeverity, const char* msg, const char* file, int line)
{
	printf("Flex: %s - %s:%d\n", msg, file, line);
	assert(0);
}
