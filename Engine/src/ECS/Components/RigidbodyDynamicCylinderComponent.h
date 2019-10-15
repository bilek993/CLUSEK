#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicCylinderComponent final
{
	float Width;
	float Radius;
	float Density;

	physx::PxRigidStatic* Body;
};
