#include "PhysicsFilterShader.h"

physx::PxFilterFlags PhysicsFilterShader::VehicleFilterShader(physx::PxFilterObjectAttributes attributes0,
	const physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, const physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
	{
		if (filterData0.word0 != 0 && filterData1.word0 != 0)
			return physx::PxFilterFlag::eSUPPRESS;
	}

	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= physx::PxPairFlags(physx::PxU16(filterData0.word2 | filterData1.word2));

	return physx::PxFilterFlags();
}
