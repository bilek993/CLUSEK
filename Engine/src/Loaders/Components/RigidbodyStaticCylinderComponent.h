#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticCylinderComponent final
{
	float Width;
	float Radius;

	physx::PxRigidStatic* Body;
};
