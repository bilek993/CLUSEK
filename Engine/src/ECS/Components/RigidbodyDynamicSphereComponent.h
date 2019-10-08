#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicSphereComponent final
{
	float Radius;
	float Density;

	physx::PxRigidDynamic* Body;
};
