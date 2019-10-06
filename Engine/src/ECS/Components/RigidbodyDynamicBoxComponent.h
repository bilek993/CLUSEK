#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicBoxComponent final
{
	float Width;
	float Height;
	float Depth;
	float Density;

	physx::PxRigidDynamic* Body;
};
