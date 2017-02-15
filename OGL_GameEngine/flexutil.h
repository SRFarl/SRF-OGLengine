#ifndef _FLEXUTIL_H_
#define _FLEXUTIL_H_

#include "Headers.h"
#include "Model.h"
#include "EntityComponents.h"

namespace flexutil
{
	void CreateSoftBody(Model* modelAsset, FlexRigidComponent* flexBase);

	Mesh SetModelToFlexMesh(Model *modelAsset);

	void CalculateRigidOffsets(const Vec4* restPositions, const int* offsets, const int* indices, int numRigids, Vec3* localPositions);

	void FindSoftBodyOffsets(FlexRigidComponent* flexRigid, RenderComponent* renderComp);

	void MakeBaseStaticInCorners(FlexRigidComponent* flexRigid);

	void ErrorCallback(FlexErrorSeverity, const char* msg, const char* file, int line);
}


#endif