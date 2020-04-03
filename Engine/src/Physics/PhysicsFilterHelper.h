#pragma once
#include "PxPhysicsAPI.h"
#include "SurfaceFilterTypes.h"
#include <string>
#include "CollisionFilterTypes.h"

class PhysicsFilterHelper final
{
public:
	static void SetQueryFilterDataForActor(physx::PxRigidActor& actor, const physx::PxFilterData& filterData);
	static void SetSimulationFilterDataForActor(physx::PxRigidActor& actor, const physx::PxFilterData& filterData);

	static void SetSurfaceFilter(physx::PxFilterData& filterData, SurfaceFilterTypes type);
	static void SetSurfaceFilter(physx::PxFilterData& filterData, const std::string& type);

	static void SetSimulationFilter(physx::PxFilterData& filterData, CollisionFilterTypes collide, CollisionFilterTypes collideAgainst);
};
