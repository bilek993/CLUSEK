#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticBoxComponent final
{
	float Width;
	float Height;
	float Depth;

	physx::PxRigidStatic* Body;
};
