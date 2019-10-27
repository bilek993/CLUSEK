#pragma once
#include "PxPhysicsAPI.h"
#include "SurfaceFilterTypes.h"
#include <string>

class PhysicsFilterHelper final
{
public:
	static void SetQueryFilterDataForShape(physx::PxRigidActor& actor, const physx::PxFilterData& filterData);

	static void SetSurfaceFilter(physx::PxFilterData& filterData, SurfaceFilterTypes type);
	static void SetSurfaceFilter(physx::PxFilterData& filterData, const std::string& type);
};
