#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticSphereComponent final
{
	float Radius;

	physx::PxRigidStatic* Body;
};
