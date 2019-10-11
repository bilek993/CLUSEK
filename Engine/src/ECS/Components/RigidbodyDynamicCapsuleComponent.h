#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicCapsuleComponent final
{
	float Height;
	float Radius;
	float Density;

	physx::PxRigidDynamic* Body;
};
