#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticCapsuleComponent final
{
	float Height;
	float Radius;

	physx::PxRigidStatic* Body;
};
